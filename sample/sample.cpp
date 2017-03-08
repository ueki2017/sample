// sample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#pragma comment(lib, "opengl32.lib")

// ���b�V���̕��ƍ���
const int width(256), height(256);

//
// �ϊ��s��
//
class Matrix
{
  // �ϊ��s��̗v�f
  GLfloat matrix[16];

public:

  // �R���X�g���N�^
  Matrix() {}

  // �z��̓��e�ŏ���������R���X�g���N�^
  //   a: GLfloat �^�� 16 �v�f�̔z��
  Matrix(const GLfloat *a)
  {
    std::copy(a, a + 16, matrix);
  }

  // �ϊ��s��̔z���Ԃ�
  const GLfloat *get() const
  {
    return matrix;
  }

  // ��Z
  //   m: Matrix �^�̏搔
  Matrix operator*(const Matrix &m) const
  {
    Matrix t;

    for (int i = 0; i < 16; ++i)
    {
      const int j(i & 3), k(i & ~3);

      t.matrix[i] =
        matrix[0 + j] * m.matrix[k + 0] +
        matrix[4 + j] * m.matrix[k + 1] +
        matrix[8 + j] * m.matrix[k + 2] +
        matrix[12 + j] * m.matrix[k + 3];
    }

    return t;
  }

  // �P�ʍs����쐬����
  static Matrix identity()
  {
    Matrix t;

    std::fill(t.matrix, t.matrix + 16, 0.0f);
    t.matrix[0] = t.matrix[5] = t.matrix[10] = t.matrix[15] = 1.0f;

    return t;
  }

  // (x, y, z) �������s�ړ�����ϊ��s����쐬����
  static Matrix translate(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t(identity());

    t.matrix[12] = x;
    t.matrix[13] = y;
    t.matrix[14] = z;

    return t;
  }

  // (x, y, z) �{�Ɋg��k������ϊ��s����쐬����
  static Matrix scale(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t(identity());

    t.matrix[0] = x;
    t.matrix[5] = y;
    t.matrix[10] = z;

    return t;
  }

  // (x, y, z) ������ a ��]����ϊ��s����쐬����
  static Matrix rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t(identity());
    const GLfloat d(sqrt(x * x + y * y + z * z));

    if (d > 0.0f)
    {
      const GLfloat l(x / d), m(y / d), n(z / d);
      const GLfloat l2(l * l), m2(m * m), n2(n * n);
      const GLfloat lm(l * m), mn(m * n), nl(n * l);
      const GLfloat c(cos(a)), c1(1.0f - c), s(sin(a));

      t.matrix[0] = (1.0f - l2) * c + l2;
      t.matrix[1] = lm * c1 + n * s;
      t.matrix[2] = nl * c1 - m * s;

      t.matrix[4] = lm * c1 - n * s;
      t.matrix[5] = (1.0f - m2) * c + m2;
      t.matrix[6] = mn * c1 + l * s;

      t.matrix[8] = nl * c1 + m * s;
      t.matrix[9] = mn * c1 - l * s;
      t.matrix[10] = (1.0f - n2) * c + n2;
    }

    return t;
  }

  // �r���[�ϊ��s����쐬����
  static Matrix lookat(
    GLfloat ex, GLfloat ey, GLfloat ez,   // ���_�̈ʒu
    GLfloat gx, GLfloat gy, GLfloat gz,   // �ڕW�_�̈ʒu
    GLfloat ux, GLfloat uy, GLfloat uz)   // ������̃x�N�g��
  {
    // ���s�ړ��̕ϊ��s��
    const Matrix tv(translate(-ex, -ey, -ez));

    // t �� = e - g
    const GLfloat tx(ex - gx);
    const GLfloat ty(ey - gy);
    const GLfloat tz(ez - gz);

    // r �� = u x t ��
    const GLfloat rx(uy * tz - uz * ty);
    const GLfloat ry(uz * tx - ux * tz);
    const GLfloat rz(ux * ty - uy * tx);

    // s �� = t �� x r ��
    const GLfloat sx(ty * rz - tz * ry);
    const GLfloat sy(tz * rx - tx * rz);
    const GLfloat sz(tx * ry - ty * rx);

    // s ���̒����̃`�F�b�N
    const GLfloat s2(sx * sx + sy * sy + sz * sz);
    if (s2 == 0.0f) return tv;

    // ��]�̕ϊ��s��
    Matrix rv(identity());

    // r ���𐳋K�����Ĕz��ϐ��Ɋi�[
    const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
    rv.matrix[0] = rx / r;
    rv.matrix[4] = ry / r;
    rv.matrix[8] = rz / r;

    // s ���𐳋K�����Ĕz��ϐ��Ɋi�[
    const GLfloat s(sqrt(s2));
    rv.matrix[1] = sx / s;
    rv.matrix[5] = sy / s;
    rv.matrix[9] = sz / s;

    // t ���𐳋K�����Ĕz��ϐ��Ɋi�[
    const GLfloat t(sqrt(tx * tx + ty * ty + tz * tz));
    rv.matrix[2] = tx / t;
    rv.matrix[6] = ty / t;
    rv.matrix[10] = tz / t;

    // ���_�̕��s�ړ��̕ϊ��s��Ɏ����̉�]�̕ϊ��s����悶��
    return rv * tv;
  }

  // ���𓊉e�ϊ��s����쐬����
  static Matrix orthogonal(GLfloat left, GLfloat right,
    GLfloat bottom, GLfloat top,
    GLfloat zNear, GLfloat zFar)
  {
    Matrix t(identity());
    const GLfloat dx(right - left);
    const GLfloat dy(top - bottom);
    const GLfloat dz(zFar - zNear);

    if (dx != 0.0f && dy != 0.0f && dz != 0.0f)
    {
      t.matrix[0] = 2.0f / dx;
      t.matrix[5] = 2.0f / dy;
      t.matrix[10] = -2.0f / dz;
      t.matrix[12] = -(right + left) / dx;
      t.matrix[13] = -(top + bottom) / dy;
      t.matrix[14] = -(zFar + zNear) / dz;
    }

    return t;
  }

  // �������e�ϊ��s����쐬����
  static Matrix frustum(GLfloat left, GLfloat right,
    GLfloat bottom, GLfloat top,
    GLfloat zNear, GLfloat zFar)
  {
    Matrix t(identity());
    const GLfloat dx(right - left);
    const GLfloat dy(top - bottom);
    const GLfloat dz(zFar - zNear);

    if (dx != 0.0f && dy != 0.0f && dz != 0.0f)
    {
      t.matrix[0] = 2.0f * zNear / dx;
      t.matrix[5] = 2.0f * zNear / dy;
      t.matrix[8] = (right + left) / dx;
      t.matrix[9] = (top + bottom) / dy;
      t.matrix[10] = -(zFar + zNear) / dz;
      t.matrix[11] = -1.0f;
      t.matrix[14] = -2.0f * zFar * zNear / dz;
      t.matrix[15] = 0.0f;
    }

    return t;
  }

  // ��p���w�肵�ē������e�ϊ��s����쐬����
  static Matrix perspective(GLfloat fovy, GLfloat aspect,
    GLfloat zNear, GLfloat zFar)
  {
    Matrix t(identity());
    const GLfloat dz(zFar - zNear);

    if (dz != 0.0f)
    {
      t.matrix[5] = 1.0f / tan(fovy * 0.5f);
      t.matrix[0] = t.matrix[5] / aspect;
      t.matrix[10] = -(zFar + zNear) / dz;
      t.matrix[11] = -1.0f;
      t.matrix[14] = -2.0f * zFar * zNear / dz;
      t.matrix[15] = 0.0f;
    }

    return t;
  }
};

/*
** �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
*/
static GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
  // �R���p�C�����ʂ��擾����
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

  // �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
  GLsizei bufSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1)
  {
    // �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  return static_cast<GLboolean>(status);
}

/*
** �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
*/
static GLboolean printProgramInfoLog(GLuint program)
{
  // �����N���ʂ��擾����
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;

  // �V�F�[�_�̃����N���̃��O�̒������擾����
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

  // �V�F�[�_�̃����N���̃��O�̓��e���擾����
  if (bufSize > 1)
  {
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  // �����N���ʂ�Ԃ�
  return static_cast<GLboolean>(status);
}

/*
** �V�F�[�_�̃\�[�X�t�@�C����ǂݍ��񂾃�������Ԃ�
*/
static bool readShaderSource(const char *name, std::vector<GLchar> &src)
{
  // �t�@�C������ NULL �Ȃ� false ��Ԃ�
  if (name == NULL) return false;

  // �\�[�X�t�@�C�����J��
  std::ifstream file(name, std::ios::binary);
  if (file.fail())
  {
    // �J���Ȃ�����
    MessageBox(NULL, TEXT("�V�F�[�_�̃\�[�X�t�@�C�����ǂݍ��߂܂���ł���"), TEXT("4D Sensor"), MB_OK);
    return false;
  }

  // �t�@�C���̖����Ɉړ������݈ʒu�i���t�@�C���T�C�Y�j�𓾂�
  file.seekg(0L, std::ios::end);
  GLsizei length = static_cast<GLsizei>(file.tellg());

  // �t�@�C���T�C�Y�̃��������m��
  src.resize(length + 1);

  // �t�@�C����擪����ǂݍ���
  file.seekg(0L, std::ios::beg);
  file.read(src.data(), length);
  src[length] = '\0';

  // ����
  bool status(true);

  // �ǂݍ��߂����ǂ������ׂ�
  if (file.bad())
  {
    // ���܂��ǂݍ��߂Ȃ�����
    MessageBox(NULL, TEXT("�V�F�[�_�̃\�[�X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���"), TEXT("4D Sensor"), MB_OK);
    status = false;
  }
  file.close();

  return status;
}

int _tmain(int argc, _TCHAR* argv[])
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    MessageBox(NULL, TEXT("GLFW �̏������Ɏ��s���܂���"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // �v���O�����̏I�����ɂ� glfwTerminate() �����s����
  atexit(glfwTerminate);

  // �E�B���h�E���쐬����
  GLFWwindow *const window(glfwCreateWindow(640, 480, "Sample", nullptr, nullptr));
  if (window == nullptr)
  {
    MessageBox(NULL, TEXT("GLFW �̃E�B���h�E���쐬�ł��܂���ł���"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // �쐬�����E�B���h�E�� OpenGL �̕`��Ώۂɂ���
  glfwMakeContextCurrent(window);

  // GLEW ������������
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    // GLEW �̏������Ɏ��s����
    MessageBox(NULL, TEXT("GLEW ���������ł��܂���ł���"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }
  
  // �B�ʏ���������L���ɂ���
  glEnable(GL_DEPTH_TEST);

  // �V�F�[�_�̃v���O�����I�u�W�F�N�g���쐬����
  const GLuint program(glCreateProgram());

  // �o�[�e�b�N�X�V�F�[�_�I�u�W�F�N�g���쐬����
  const GLuint vsh(glCreateShader(GL_VERTEX_SHADER));

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O����
  std::vector<GLchar> vsrc;
  readShaderSource("simple.vert", vsrc);

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O������ǂݍ���
  const GLchar *const vsrcptr[] = { vsrc.data() };
  glShaderSource(vsh, 1, vsrcptr, nullptr);

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�������R���p�C������
  glCompileShader(vsh);

  // �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
  if (printShaderInfoLog(vsh, "vertex shader"))
  {
    // �v���O�����I�u�W�F�N�g�ɃV�F�[�_�I�u�W�F�N�g�����t����
    glAttachShader(program, vsh);
  }

  // ���t���ς݂̃V�F�[�_�I�u�W�F�N�g���폜����
  glDeleteShader(vsh);

  // �t���O�����g�V�F�[�_�I�u�W�F�N�g���쐬����
  const GLuint fsh(glCreateShader(GL_FRAGMENT_SHADER));

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O����
  std::vector<GLchar> fsrc;
  readShaderSource("simple.frag", fsrc);

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O������ǂݍ���
  const GLchar *const fsrcp[] = { fsrc.data() };
  glShaderSource(fsh, 1, fsrcp, nullptr);

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O�������R���p�C������
  glCompileShader(fsh);

  // �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
  if (printShaderInfoLog(fsh, "fragment shader"))
  {
    // �v���O�����I�u�W�F�N�g�ɃV�F�[�_�I�u�W�F�N�g�����t����
    glAttachShader(program, fsh);
  }

  // ���t���ς݂̃V�F�[�_�I�u�W�F�N�g���폜����
  glDeleteShader(fsh);

  // �o�[�e�b�N�X�V�F�[�_�� in �ϐ� pv �� 0 �Ԃ̃o�b�t�@�I�u�W�F�N�g�Ɋ��蓖�Ă�
  glBindAttribLocation(program, 0, "pv");

  // �o�[�e�b�N�X�V�F�[�_�� in �ϐ� cv �� 1 �Ԃ̃o�b�t�@�I�u�W�F�N�g�Ɋ��蓖�Ă�
  glBindAttribLocation(program, 1, "cv");

  // �t���O�����g�V�F�[�_�� out �ϐ� fc �� 0 �Ԃ̃J���[�o�b�t�@�Ɋ��蓖�Ă�
  glBindFragDataLocation(program, 0, "fc");

  // �V�F�[�_�̃v���O�����I�u�W�F�N�g�������N����
  glLinkProgram(program);

  // �v���O�����I�u�W�F�N�g���쐬�ł������ǂ����`�F�b�N����
  if (printProgramInfoLog(program) == GL_FALSE)
  {
    // �쐬�ł��Ȃ������Ƃ��̓v���O�����I�u�W�F�N�g���폜����
    glDeleteProgram(program);

    // �V�F�[�_�v���O�����̃r���h�Ɏ��s����
    MessageBox(NULL, TEXT("�V�F�[�_�v���O�������r���h�ł��܂���ł���"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // uniform �ϐ��̏ꏊ�𓾂�
  const GLint mLoc(glGetUniformLocation(program, "m"));

  // ���_�z��I�u�W�F�N�g���쐬����
  GLuint vao;
  glGenVertexArrays(1, &vao);

  // ���_�z��I�u�W�F�N�g��I������
  glBindVertexArray(vao);

  // ���_�o�b�t�@�I�u�W�F�N�g���쐬����
  GLuint vbo;
  glGenBuffers(1, &vbo);

  // ���_�o�b�t�@�I�u�W�F�N�g��I������
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // ���_�f�[�^
  struct Vertex
  {
    GLfloat p[3]; // �ʒu
    GLfloat c[3]; // �F
  };

  static Vertex v[height][width];

  for (int j = 0; j < height; ++j)
  {
    const GLfloat y(2.0f * static_cast<GLfloat>(j) / static_cast<GLfloat>(height - 1) - 1.0f);
    for (int i = 0; i < width; ++i)
    {
      const GLfloat x(2.0f * static_cast<GLfloat>(i) / static_cast<GLfloat>(width - 1) - 1.0f);
      const GLfloat r(hypot(x, y) * 10.0f);
      const GLfloat z(sin(r) / r);

      // �ʒu
      v[j][i].p[0] = x;
      v[j][i].p[1] = y;
      v[j][i].p[2] = z;

      // �F
      v[j][i].c[0] = z;
      v[j][i].c[0] = 1.0f - z;
      v[j][i].c[0] = 1.0f - z;
    }
  }

  static GLuint index[height][width][2];

  for (int j = 0; j < height - 1; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      index[j][i][0] = j * width + i;
      index[j][i][1] = index[j][i][0] + width;
    }
  }

  // ���_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBufferData(GL_ARRAY_BUFFER, sizeof v, v, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof index, index, GL_STATIC_DRAW);

  // ���̒��_�o�b�t�@�I�u�W�F�N�g�̈ʒu�� 0 �Ԃ� in �ϐ��Ɋ��蓖�Ă�
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), nullptr);

  // ���� in �ϐ���L���ɂ���
  glEnableVertexAttribArray(0);

  // ���̒��_�o�b�t�@�I�u�W�F�N�g�̐F�� 1 �Ԃ� in �ϐ��Ɋ��蓖�Ă�
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), static_cast<GLfloat *>(nullptr) + 2);

  // ���� in �ϐ���L���ɂ���
  glEnableVertexAttribArray(1);

  // �w�i�F���w�肷��
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // �t���[����
  int frame(0);

  // �E�B���h�E���J���Ă���ԏ������J��Ԃ�
  while (!glfwWindowShouldClose(window))
  {
    // ��ʂ���������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �v���O�����I�u�W�F�N�g���w�肷��
    glUseProgram(program);

    // �ϊ��s����쐬����
    const GLfloat r(static_cast<GLfloat>(frame++) * 0.001f);
    Matrix m(Matrix::rotate(r, 0.0f, 0.0f, 1.0f));

    // ����ϊ��s��
    Matrix v(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));

    // ���e�ϊ��s��
    Matrix q(Matrix::perspective(0.5f, 1.0f, 1.0, 10.0f));

    // uniform �ϐ��ɒl��ݒ肷��
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, (q * v * m).get());

    // �}�`��`�悷��
    glBindVertexArray(vao);
    for (int j = 0; j < height - 1; ++j)
    {
      glDrawElements(GL_TRIANGLE_STRIP, width * 2, GL_UNSIGNED_INT, index[j]);
    }

    // �o�b�t�@�����ւ���
    glfwSwapBuffers(window);

    // �C�x���g���擾����
    glfwPollEvents();
  }

	return EXIT_SUCCESS;
}
