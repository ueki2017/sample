#version 330

// ���_�f�[�^
in vec2 pv;

// �ϊ��s��
uniform mat4 m;

void main()
{
  gl_Position = m * vec4(pv, 0.0, 1.0);
}
