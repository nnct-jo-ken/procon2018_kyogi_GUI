#include"qrdecode.h"

QREncodingMode ConvertEncodingMode(int qdata) {
	switch (qdata) {
	case 0:
		return QREncodingMode::Numeric;
	case 1:
		return QREncodingMode::Alnum;
	case 2:
		return QREncodingMode::Binary;
	case 3:
		return QREncodingMode::Kanji;
	case 4:
		return QREncodingMode::Unknown;
	}
}

bool decode(const Image& image, QRData& data) {
	struct quirc *m_quirc = quirc_new();

	if (!image || !m_quirc) {
		return false;
	}

	if (quirc_resize(m_quirc, image.width, image.height)) {
		return false;
	}

	uint8* p = quirc_begin(m_quirc, nullptr, nullptr);

	for (const auto& pixel : image) {
		*(p++) = pixel.grayscale();
	}

	quirc_end(m_quirc);

	const int num_codes = quirc_count(m_quirc);

	if (!num_codes) {
		return false;
	}

	quirc_code qcode;
	quirc_data qdata;
	quirc_decode_error_t qerr;

	quirc_extract(m_quirc, 0, &qcode);

	qerr = quirc_decode(&qcode, &qdata);

	if (qerr) {// Ç±Ç±Ç≈ÉGÉâÅ[
		return false;
	}

	for (size_t i = 0; i < 4; ++i) {
		data.quad.p[i].set(qcode.corners[i].x, qcode.corners[i].y);
	}

	data.mode = ConvertEncodingMode(qdata.data_type);
	data.ecLevel = static_cast<QRECLevel>(qdata.ecc_level);
	data.version = qdata.version;

	if (data.mode == QREncodingMode::Numeric || data.mode == QREncodingMode::Alnum) {
		data.text = CharacterSet::WidenAscii(std::string(static_cast<const char*>(static_cast<const void*>(qdata.payload)), qdata.payload_len));
	}
	else if (data.mode == QREncodingMode::Kanji) {
		data.text = CharacterSet::Widen(std::string(static_cast<const char*>(static_cast<const void*>(qdata.payload)), qdata.payload_len));
	}
	else {
		data.text = CharacterSet::FromUTF8(std::string(static_cast<const char*>(static_cast<const void*>(qdata.payload)), qdata.payload_len));
	}

	data.data = ByteArray(qdata.payload, qdata.payload_len);

	quirc_destroy(m_quirc);

	return true;
}