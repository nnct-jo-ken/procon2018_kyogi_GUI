#include "rand_board.h"

int init::rand_board(int board[12][12], int row, int column) {
	std::random_device rnd;     // �񌈒�I�ȗ���������𐶐�
	std::mt19937 mt(rnd());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	std::uniform_int_distribution<> rand_score(-16, 16 * 9);     // [-16, 16*9] �͈͂̈�l���� �}�C�i�X�̓_��10%�߂��ɂ���

	std::random_device state;     // �񌈒�I�ȗ���������𐶐�
	std::mt19937 mt_state(state());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	std::uniform_int_distribution<> rand_state(0, 2);     // 0:�����E���� 1:���� 2:����

	int status = rand_state(mt_state);	//���Ώ̂̈ʒu�𗐐��Ō���
	status = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			int tmp = rand_score(mt);
			if (tmp > 0) {     //���̗����̊��������߂�
				tmp /= 9;
			}

			board[i][j] = tmp;	//�Ƃɂ��������𐶐�

			if (status == 0 || status == 1) {	//�����ɐ��Ώ�
				if (row % 2 == 0) {	//����
					if (i > row / 2 - 1) {
						board[i][j] = board[(row - i - 1)][j];
					}
				}
				else {	//�
					if (i > row / 2) {
						board[i][j] = board[(row - i - 1)][j];
					}
				}
			}

			if (status == 0 || status == 2) {	//�����ɐ��Ώ�
				if (column % 2 == 0) {	//����
					if (j > column / 2 - 1) {
						board[i][j] = board[i][column - j - 1];
					}
				}
				else {	//�
					if (j > column / 2) {
						board[i][column + j] = board[i][column - j - 1];
					}
				}
			}

		}
	}
	return 0;
}