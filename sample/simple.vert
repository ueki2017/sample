#version 330

// 頂点データ
in vec2 pv;

// 変換行列
uniform mat4 m;

void main()
{
  gl_Position = m * vec4(pv, 0.0, 1.0);
}
