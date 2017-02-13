// sample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#pragma comment(lib, "opengl32.lib")

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
  
  // バーテックスシェーダオブジェクトを作成する
  const GLuint vsh(glCreateShader(GL_VERTEX_SHADER));

  // バーテックスシェーダのソースプログラム
  GLchar *vsrc[] =
  {
    "#version 330\n",
    "",
    "// 頂点データ",
    "int vec2 position;",
    "",
    "void main()",
    "{",
    "  gl_Position = vec4(pv, 0.0, 1.0);",
    "}"
  };

  // バーテックスシェーダのソースプログラムを読み込む
  glShaderSource(vsh, sizeof vsrc / sizeof vsrc[0], vsrc, nullptr);

  // バーテックスシェーダのソースプログラムをコンパイルする
  glCompileShader(vsh);

  // フラグメントシェーダオブジェクトを作成する
  const GLuint fsh(glCreateShader(GL_FRAGMENT_SHADER));

  // フラグメントシェーダのソースプログラム
  GLchar *fsrc[] =
  {
    "#version 330\n",
    "",
    "// 画素データ",
    "out vec4 fc;",
    "",
    "void main()",
    "{",
    "  fc = vec4(1.0, 1.0, 0.0, 1.0);",
    "}"
  };

  // フラグメントシェーダのソースプログラムを読み込む
  glShaderSource(fsh, sizeof fsrc / sizeof fsrc[0], fsrc, nullptr);

  // フラグメントシェーダのソースプログラムをコンパイルする
  glCompileShader(fsh);

  // シェーダのプログラムオブジェクトを作成する
  const GLuint program(glCreateProgram());

  // プログラムオブジェクトにシェーダオブジェクトを取り付ける
  glAttachShader(program, vsh);
  glAttachShader(program, fsh);

  // 取り付け済みのシェーダオブジェクトを削除する
  glDeleteShader(vsh);
  glDeleteShader(fsh);

  // バーテックスシェーダの in 変数 pv を 0 番のバッファオブジェクトに割り当てる
  glBindAttribLocation(program, 0, "pv");

  // フラグメントシェーダの out 変数 fc を 0 番のカラーバッファに割り当てる
  glBindFragDataLocation(program, 0, "fc");

  // シェーダのプログラムオブジェクトをリンクする
  glLinkProgram(program);

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

  // ウィンドウが開いている間処理を繰り返す
  while (!glfwWindowShouldClose(window))
  {
    // 画面を消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // プログラムオブジェクトを指定する
    glUseProgram(program);

    // 図形を描画する
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // バッファを入れ替える
    glfwSwapBuffers(window);

    // イベントの発生を待つ
    glfwWaitEvents();
  }

	return EXIT_SUCCESS;
}
