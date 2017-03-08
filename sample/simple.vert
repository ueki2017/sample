#version 330

// 頂点データ
in vec4 pv;
in vec3 cv;

// 変換行列
uniform mat4 m;

// ラスタライザで補間してもらうデータ
out vec3 vc;

void main()
{
  gl_Position = m * pv;
  vc = cv;
}
