#version 330

// ���_�f�[�^
in vec2 pv;

void main()
{
  gl_Position = vec4(pv, 0.0, 1.0);
}
