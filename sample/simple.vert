#version 330

// ���_�f�[�^
in vec4 pv;

// �ϊ��s��
uniform mat4 m;

void main()
{
  gl_Position = m * pv;
}
