// sample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#pragma comment(lib, "opengl32.lib")

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
  static GLfloat p[][2] =
  {
    { -0.5f, -0.5f },
    {  0.5f, -0.5f },
    {  0.5f,  0.5f },
    { -0.5f,  0.5f }
  };

  // 頂点バッファオブジェクトのメモリを確保する
  glBufferData(GL_ARRAY_BUFFER, sizeof p, p, GL_STATIC_DRAW);

  // この頂点バッファオブジェクトを 0 番の in 変数に割り当てる
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // この in 変数を有効にする
  glEnableVertexAttribArray(0);

  // 背景色を指定する
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // フレーム数
  int frame(0);

  // ウィンドウが開いている間処理を繰り返す
  while (!glfwWindowShouldClose(window))
  {
    // 画面を消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // プログラムオブジェクトを指定する
    glUseProgram(program);

    // 変換行列を作成する
    const GLfloat r(static_cast<GLfloat>(frame++) * 0.001f);
    const GLfloat m[] =
    {
      cos(r), -sin(r), 0.0, 0.0,
      sin(r), cos(r), 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0
    };

    // uniform 変数に値を設定する
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, m);

    // 図形を描画する
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // バッファを入れ替える
    glfwSwapBuffers(window);

    // イベントを取得する
    glfwPollEvents();
  }

	return EXIT_SUCCESS;
}
