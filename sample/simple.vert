#version 330

// 頂点データ
in vec4 pv;

// 変換行列
uniform mat4 m;

void main()
{
  gl_Position = m * pv;
}
