// sample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#pragma comment(lib, "opengl32.lib")

// メッシュの幅と高さ
const int width(256), height(256);

//
// 変換行列
//
class Matrix
{
  // 変換行列の要素
  GLfloat matrix[16];

public:

  // コンストラクタ
  Matrix() {}

  // 配列の内容で初期化するコンストラクタ
  //   a: GLfloat 型の 16 要素の配列
  Matrix(const GLfloat *a)
  {
    std::copy(a, a + 16, matrix);
  }

  // 変換行列の配列を返す
  const GLfloat *get() const
  {
    return matrix;
  }

  // 乗算
  //   m: Matrix 型の乗数
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

  // 単位行列を作成する
  static Matrix identity()
  {
    Matrix t;

    std::fill(t.matrix, t.matrix + 16, 0.0f);
    t.matrix[0] = t.matrix[5] = t.matrix[10] = t.matrix[15] = 1.0f;

    return t;
  }

  // (x, y, z) だけ平行移動する変換行列を作成する
  static Matrix translate(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t(identity());

    t.matrix[12] = x;
    t.matrix[13] = y;
    t.matrix[14] = z;

    return t;
  }

  // (x, y, z) 倍に拡大縮小する変換行列を作成する
  static Matrix scale(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t(identity());

    t.matrix[0] = x;
    t.matrix[5] = y;
    t.matrix[10] = z;

    return t;
  }

  // (x, y, z) を軸に a 回転する変換行列を作成する
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

  // ビュー変換行列を作成する
  static Matrix lookat(
    GLfloat ex, GLfloat ey, GLfloat ez,   // 視点の位置
    GLfloat gx, GLfloat gy, GLfloat gz,   // 目標点の位置
    GLfloat ux, GLfloat uy, GLfloat uz)   // 上方向のベクトル
  {
    // 平行移動の変換行列
    const Matrix tv(translate(-ex, -ey, -ez));

    // t 軸 = e - g
    const GLfloat tx(ex - gx);
    const GLfloat ty(ey - gy);
    const GLfloat tz(ez - gz);

    // r 軸 = u x t 軸
    const GLfloat rx(uy * tz - uz * ty);
    const GLfloat ry(uz * tx - ux * tz);
    const GLfloat rz(ux * ty - uy * tx);

    // s 軸 = t 軸 x r 軸
    const GLfloat sx(ty * rz - tz * ry);
    const GLfloat sy(tz * rx - tx * rz);
    const GLfloat sz(tx * ry - ty * rx);

    // s 軸の長さのチェック
    const GLfloat s2(sx * sx + sy * sy + sz * sz);
    if (s2 == 0.0f) return tv;

    // 回転の変換行列
    Matrix rv(identity());

    // r 軸を正規化して配列変数に格納
    const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
    rv.matrix[0] = rx / r;
    rv.matrix[4] = ry / r;
    rv.matrix[8] = rz / r;

    // s 軸を正規化して配列変数に格納
    const GLfloat s(sqrt(s2));
    rv.matrix[1] = sx / s;
    rv.matrix[5] = sy / s;
    rv.matrix[9] = sz / s;

    // t 軸を正規化して配列変数に格納
    const GLfloat t(sqrt(tx * tx + ty * ty + tz * tz));
    rv.matrix[2] = tx / t;
    rv.matrix[6] = ty / t;
    rv.matrix[10] = tz / t;

    // 視点の平行移動の変換行列に視線の回転の変換行列を乗じる
    return rv * tv;
  }

  // 直交投影変換行列を作成する
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

  // 透視投影変換行列を作成する
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

  // 画角を指定して透視投影変換行列を作成する
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
** シェーダオブジェクトのコンパイル結果を表示する
*/
static GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
  // コンパイル結果を取得する
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

  // シェーダのコンパイル時のログの長さを取得する
  GLsizei bufSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1)
  {
    // シェーダのコンパイル時のログの内容を取得する
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  return static_cast<GLboolean>(status);
}

/*
** プログラムオブジェクトのリンク結果を表示する
*/
static GLboolean printProgramInfoLog(GLuint program)
{
  // リンク結果を取得する
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;

  // シェーダのリンク時のログの長さを取得する
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

  // シェーダのリンク時のログの内容を取得する
  if (bufSize > 1)
  {
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  // リンク結果を返す
  return static_cast<GLboolean>(status);
}

/*
** シェーダのソースファイルを読み込んだメモリを返す
*/
static bool readShaderSource(const char *name, std::vector<GLchar> &src)
{
  // ファイル名が NULL なら false を返す
  if (name == NULL) return false;

  // ソースファイルを開く
  std::ifstream file(name, std::ios::binary);
  if (file.fail())
  {
    // 開けなかった
    MessageBox(NULL, TEXT("シェーダのソースファイルが読み込めませんでした"), TEXT("4D Sensor"), MB_OK);
    return false;
  }

  // ファイルの末尾に移動し現在位置（＝ファイルサイズ）を得る
  file.seekg(0L, std::ios::end);
  GLsizei length = static_cast<GLsizei>(file.tellg());

  // ファイルサイズのメモリを確保
  src.resize(length + 1);

  // ファイルを先頭から読み込む
  file.seekg(0L, std::ios::beg);
  file.read(src.data(), length);
  src[length] = '\0';

  // 結果
  bool status(true);

  // 読み込めたかどうか調べる
  if (file.bad())
  {
    // うまく読み込めなかった
    MessageBox(NULL, TEXT("シェーダのソースファイルの読み込みに失敗しました"), TEXT("4D Sensor"), MB_OK);
    status = false;
  }
  file.close();

  return status;
}

int _tmain(int argc, _TCHAR* argv[])
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    MessageBox(NULL, TEXT("GLFW の初期化に失敗しました"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // プログラムの終了時には glfwTerminate() を実行する
  atexit(glfwTerminate);

  // ウィンドウを作成する
  GLFWwindow *const window(glfwCreateWindow(640, 480, "Sample", nullptr, nullptr));
  if (window == nullptr)
  {
    MessageBox(NULL, TEXT("GLFW のウィンドウが作成できませんでした"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // 作成したウィンドウを OpenGL の描画対象にする
  glfwMakeContextCurrent(window);

  // GLEW を初期化する
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    // GLEW の初期化に失敗した
    MessageBox(NULL, TEXT("GLEW が初期化できませんでした"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }
  
  // 隠面消去処理を有効にする
  glEnable(GL_DEPTH_TEST);

  // シェーダのプログラムオブジェクトを作成する
  const GLuint program(glCreateProgram());

  // バーテックスシェーダオブジェクトを作成する
  const GLuint vsh(glCreateShader(GL_VERTEX_SHADER));

  // バーテックスシェーダのソースプログラム
  std::vector<GLchar> vsrc;
  readShaderSource("simple.vert", vsrc);

  // バーテックスシェーダのソースプログラムを読み込む
  const GLchar *const vsrcptr[] = { vsrc.data() };
  glShaderSource(vsh, 1, vsrcptr, nullptr);

  // バーテックスシェーダのソースプログラムをコンパイルする
  glCompileShader(vsh);

  // バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
  if (printShaderInfoLog(vsh, "vertex shader"))
  {
    // プログラムオブジェクトにシェーダオブジェクトを取り付ける
    glAttachShader(program, vsh);
  }

  // 取り付け済みのシェーダオブジェクトを削除する
  glDeleteShader(vsh);

  // フラグメントシェーダオブジェクトを作成する
  const GLuint fsh(glCreateShader(GL_FRAGMENT_SHADER));

  // フラグメントシェーダのソースプログラム
  std::vector<GLchar> fsrc;
  readShaderSource("simple.frag", fsrc);

  // フラグメントシェーダのソースプログラムを読み込む
  const GLchar *const fsrcp[] = { fsrc.data() };
  glShaderSource(fsh, 1, fsrcp, nullptr);

  // フラグメントシェーダのソースプログラムをコンパイルする
  glCompileShader(fsh);

  // フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
  if (printShaderInfoLog(fsh, "fragment shader"))
  {
    // プログラムオブジェクトにシェーダオブジェクトを取り付ける
    glAttachShader(program, fsh);
  }

  // 取り付け済みのシェーダオブジェクトを削除する
  glDeleteShader(fsh);

  // バーテックスシェーダの in 変数 pv を 0 番のバッファオブジェクトに割り当てる
  glBindAttribLocation(program, 0, "pv");

  // バーテックスシェーダの in 変数 cv を 1 番のバッファオブジェクトに割り当てる
  glBindAttribLocation(program, 1, "cv");

  // フラグメントシェーダの out 変数 fc を 0 番のカラーバッファに割り当てる
  glBindFragDataLocation(program, 0, "fc");

  // シェーダのプログラムオブジェクトをリンクする
  glLinkProgram(program);

  // プログラムオブジェクトが作成できたかどうかチェックする
  if (printProgramInfoLog(program) == GL_FALSE)
  {
    // 作成できなかったときはプログラムオブジェクトを削除する
    glDeleteProgram(program);

    // シェーダプログラムのビルドに失敗した
    MessageBox(NULL, TEXT("シェーダプログラムがビルドできませんでした"), TEXT("4D Sensor"), MB_OK);
    return EXIT_FAILURE;
  }

  // uniform 変数の場所を得る
  const GLint mLoc(glGetUniformLocation(program, "m"));

  // 頂点配列オブジェクトを作成する
  GLuint vao;
  glGenVertexArrays(1, &vao);

  // 頂点配列オブジェクトを選択する
  glBindVertexArray(vao);

  // 頂点バッファオブジェクトを作成する
  GLuint vbo;
  glGenBuffers(1, &vbo);

  // 頂点バッファオブジェクトを選択する
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // 頂点データ
  struct Vertex
  {
    GLfloat p[3]; // 位置
    GLfloat c[3]; // 色
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

      // 位置
      v[j][i].p[0] = x;
      v[j][i].p[1] = y;
      v[j][i].p[2] = z;

      // 色
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

  // 頂点バッファオブジェクトのメモリを確保する
  glBufferData(GL_ARRAY_BUFFER, sizeof v, v, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof index, index, GL_STATIC_DRAW);

  // この頂点バッファオブジェクトの位置を 0 番の in 変数に割り当てる
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), nullptr);

  // この in 変数を有効にする
  glEnableVertexAttribArray(0);

  // この頂点バッファオブジェクトの色を 1 番の in 変数に割り当てる
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), static_cast<GLfloat *>(nullptr) + 2);

  // この in 変数を有効にする
  glEnableVertexAttribArray(1);

  // 背景色を指定する
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // フレーム数
  int frame(0);

  // ウィンドウが開いている間処理を繰り返す
  while (!glfwWindowShouldClose(window))
  {
    // 画面を消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // プログラムオブジェクトを指定する
    glUseProgram(program);

    // 変換行列を作成する
    const GLfloat r(static_cast<GLfloat>(frame++) * 0.001f);
    Matrix m(Matrix::rotate(r, 0.0f, 0.0f, 1.0f));

    // 視野変換行列
    Matrix v(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));

    // 投影変換行列
    Matrix q(Matrix::perspective(0.5f, 1.0f, 1.0, 10.0f));

    // uniform 変数に値を設定する
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, (q * v * m).get());

    // 図形を描画する
    glBindVertexArray(vao);
    for (int j = 0; j < height - 1; ++j)
    {
      glDrawElements(GL_TRIANGLE_STRIP, width * 2, GL_UNSIGNED_INT, index[j]);
    }

    // バッファを入れ替える
    glfwSwapBuffers(window);

    // イベントを取得する
    glfwPollEvents();
  }

	return EXIT_SUCCESS;
}
