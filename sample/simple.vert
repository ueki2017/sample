#version 330

// ���_�f�[�^
in vec4 pv;
in vec3 cv;

// �ϊ��s��
uniform mat4 m;

// ���X�^���C�U�ŕ�Ԃ��Ă��炤�f�[�^
out vec3 vc;

void main()
{
  gl_Position = m * pv;
  vc = cv;
}
