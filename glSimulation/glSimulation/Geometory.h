#pragma once
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

class Geometory
{
public:
	// �T�b�J�[�h�~���[�̊p�x���^�������, MVP ���X�V
	// tilt, pan �͂��ꂼ��f�t�H�� 45 �x�ł���Ƃ��� deg �̒P�ʂœn���Ă�
	void update(const float alpha, const float beta);
	Geometory(const cv::Mat1d& Tp, const cv::Mat1d& Rc, const cv::Mat1d& K, const cv::Size camSz, const float L);
	const glm::mat4 getMVP() const;
private:
	// �~���[��]���ԋ���
	const float L;
	// �~���[��p(tilt, pan)
	const float alpha0, beta0;
	// �~���[�p�X�P�[��
	const float tilt_scale, pan_scale;
	// �~���[�\�ʌX��
	const float gamma_t, gamma_p;
	// Tilt �~���[��]
	const float r_tilt;
	// �J�������i
	const cv::Mat1d Tp;
	// �J������]
	const cv::Mat1d Rc;
	// �~���[����
	const float d;
	// Projection Matrix
	const cv::Mat1d Projection;
	// Model View Projection Matrix
	glm::mat4 MVP;
	bool first;
};

namespace SMMath {
	/* �v���~�e�B�u�Ȋ֐� */

	// x ���܂��̉�]
	// ���́F��]�p
	cv::Mat1d Rx(const double deg);
	// y ���܂��̉�]
	// ���́F��]�p
	cv::Mat1d Ry(const double deg);
	// z ���܂��̉�]
	// ���́F��]�p
	cv::Mat1d Rz(const double deg);
	// ��]�s��
	// ���́Fx ����], y ����], z ����]
	cv::Mat1d R(const double deg_x, const double deg_y, const double deg_z);
	// x �����ւ̕��i�x�N�g��
	cv::Mat1d Tx(const double l);

	/* �����ϊ��s�� */

	// ��]�s��
	// ���́F�@���x�N�g��
	cv::Mat1d Mr(const cv::Point3d& n);

	// ���s�ړ��x�N�g��
	// ���́F�@���x�N�g��, ���_����̋���
	cv::Mat1d Mt(const cv::Point3d& n, const double k);

	// �A�t�B���ϊ�
	// ���́F�@���x�N�g��, ���_����̋���
	cv::Mat1d M(const cv::Point3d& n, const double k);

	/* �@���x�N�g���֘A */
	// Tilt mirror �̖@���x�N�g��
	cv::Point3d nt(const double alpha_deg, const double surf_deg, const double rtilt1_deg, const double rtilt2_deg, const double rtilt3_deg);
	// Pan mirror �̖@���x�N�g��
	cv::Point3d np(const double beta_deg, const double surf_deg, const double rpan1_deg, const double rpan2_deg, const double rpan3_deg);

	// �J������񂩂� Projection Matrix �����
	cv::Mat1d Projection(const cv::Mat1d& camMat, const cv::Size camSz, const double znear, const double zfar);

	// ��]�ƕ��s�ړ����w�肵�� 4*4 �̃A�t�B���s������
	cv::Mat1d Affine(const cv::Mat1d R, const cv::Mat1d t);
}
