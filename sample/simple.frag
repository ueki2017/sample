#version 330

// ラスタライザで補間してもらったデータ
in vec3 vc;

// 画素データ
out vec4 fc;

void main()
{
  fc = vec4(vc, 1.0);
}
