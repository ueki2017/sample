#version 330

// 頂点データ
in vec2 pv;

void main()
{
  gl_Position = vec4(pv, 0.0, 1.0);
}
