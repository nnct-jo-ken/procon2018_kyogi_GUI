#include "QR_reader.h"

void QR_reader::init()
{
	message = GUI(GUIStyle::Default);	//���E�B���h�E
	message.setTitle(L"���b�Z�[�W");
	message.setPos(650, 10);
	message.add(L"tf1", GUITextField::Create(none));

	confirmation = GUI(GUIStyle::Default);	//�m�F�E�B���h�E
	confirmation.setTitle(L"����ŗǂ��ł���?");
	confirmation.add(L"yes", GUIButton::Create(L"Yes"));
	confirmation.add(L"no", GUIButton::Create(L"No"));
	confirmation.setPos(650, 100);
	confirmation.show(false);

	message.hide();
	confirmation.hide();
}

void QR_reader::read()
{
	Webcam webcam;
	confirmation.show();
	message.show();

	if (!webcam.open(0, Size(640, 480))) {
		return;
	}

	if (!webcam.start()) {
		return;
	}

	while (System::Update()) {
		Optional<Quad> quad;

		// x�{�^�����������Ƃ��̏���
		if (System::GetPreviousEvent() & WindowEvent::CloseButton)
		{
			break;
		}

		if (webcam.hasNewFrame()) {
			message.textField(L"tf1").setText(L"�ǂݎ�蒆");
			webcam.getFrame(image);

			texture.fill(image);

			if (decode(image, data)) {
				webcam.pause();
				binary.resize(data.data.size());
				data.data.read(&binary[0], data.data.size());
				decoded.push_back(Widen(binary));
			}
		}

		if (!webcam.isActive() && webcam.isOpened()) {
			message.textField(L"tf1").setText(L"�F�����܂���");
			quad = data.quad;

			confirmation.show(true);
			if (confirmation.button(L"yes").pressed) {

				confirmation.hide();
				message.hide();

				message.textField(L"tf1").setText(L"�o�͂��܂���");
				break;
			}
			if (confirmation.button(L"no").pressed) {
				confirmation.show(false);
				webcam.resume();

				do {
					webcam.getFrame(image);
				} while (decode(image, data));
			}
		}

		if (texture) {
			texture.draw();
		}

		if (quad)
		{
			quad->drawFrame(4, Palette::Red);
		}
	}
}