// sample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#pragma comment(lib, "opengl32.lib")

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
  
  // �o�[�e�b�N�X�V�F�[�_�I�u�W�F�N�g���쐬����
  const GLuint vsh(glCreateShader(GL_VERTEX_SHADER));

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O����
  GLchar *vsrc[] =
  {
    "#version 330\n",
    "",
    "// ���_�f�[�^",
    "int vec2 position;",
    "",
    "void main()",
    "{",
    "  gl_Position = vec4(pv, 0.0, 1.0);",
    "}"
  };

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O������ǂݍ���
  glShaderSource(vsh, sizeof vsrc / sizeof vsrc[0], vsrc, nullptr);

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�������R���p�C������
  glCompileShader(vsh);

  // �t���O�����g�V�F�[�_�I�u�W�F�N�g���쐬����
  const GLuint fsh(glCreateShader(GL_FRAGMENT_SHADER));

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O����
  GLchar *fsrc[] =
  {
    "#version 330\n",
    "",
    "// ��f�f�[�^",
    "out vec4 fc;",
    "",
    "void main()",
    "{",
    "  fc = vec4(1.0, 1.0, 0.0, 1.0);",
    "}"
  };

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O������ǂݍ���
  glShaderSource(fsh, sizeof fsrc / sizeof fsrc[0], fsrc, nullptr);

  // �t���O�����g�V�F�[�_�̃\�[�X�v���O�������R���p�C������
  glCompileShader(fsh);

  // �V�F�[�_�̃v���O�����I�u�W�F�N�g���쐬����
  const GLuint program(glCreateProgram());

  // �v���O�����I�u�W�F�N�g�ɃV�F�[�_�I�u�W�F�N�g�����t����
  glAttachShader(program, vsh);
  glAttachShader(program, fsh);

  // ���t���ς݂̃V�F�[�_�I�u�W�F�N�g���폜����
  glDeleteShader(vsh);
  glDeleteShader(fsh);

  // �o�[�e�b�N�X�V�F�[�_�� in �ϐ� pv �� 0 �Ԃ̃o�b�t�@�I�u�W�F�N�g�Ɋ��蓖�Ă�
  glBindAttribLocation(program, 0, "pv");

  // �t���O�����g�V�F�[�_�� out �ϐ� fc �� 0 �Ԃ̃J���[�o�b�t�@�Ɋ��蓖�Ă�
  glBindFragDataLocation(program, 0, "fc");

  // �V�F�[�_�̃v���O�����I�u�W�F�N�g�������N����
  glLinkProgram(program);

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
  static GLfloat p[][2] =
  {
    { -0.5f, -0.5f },
    {  0.5f, -0.5f },
    {  0.5f,  0.5f },
    { -0.5f,  0.5f }
  };

  // ���_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBufferData(GL_ARRAY_BUFFER, sizeof p, p, GL_STATIC_DRAW);

  // ���̒��_�o�b�t�@�I�u�W�F�N�g�� 0 �Ԃ� in �ϐ��Ɋ��蓖�Ă�
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // ���� in �ϐ���L���ɂ���
  glEnableVertexAttribArray(0);

  // �w�i�F���w�肷��
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // �E�B���h�E���J���Ă���ԏ������J��Ԃ�
  while (!glfwWindowShouldClose(window))
  {
    // ��ʂ���������
    glClear(GL_COLOR_BUFFER_BIT);

    // �v���O�����I�u�W�F�N�g���w�肷��
    glUseProgram(program);

    // �}�`��`�悷��
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // �o�b�t�@�����ւ���
    glfwSwapBuffers(window);

    // �C�x���g�̔�����҂�
    glfwWaitEvents();
  }

	return EXIT_SUCCESS;
}
