#pragma once
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

class Geometory
{
public:
	// サッカードミラーの角度が与えられると, MVP を更新
	// tilt, pan はそれぞれデフォで 45 度であるとして deg の単位で渡してね
	void update(const float alpha, const float beta);
	Geometory(const cv::Mat1d& Tp, const cv::Mat1d& Rc, const cv::Mat1d& K, const cv::Size camSz, const float L);
	const glm::mat4 getMVP() const;
private:
	// ミラー回転軸間距離
	const float L;
	// ミラー基準角(tilt, pan)
	const float alpha0, beta0;
	// ミラー角スケール
	const float tilt_scale, pan_scale;
	// ミラー表面傾斜
	const float gamma_t, gamma_p;
	// Tilt ミラー回転
	const float r_tilt;
	// カメラ並進
	const cv::Mat1d Tp;
	// カメラ回転
	const cv::Mat1d Rc;
	// ミラー厚み
	const float d;
	// Projection Matrix
	const cv::Mat1d Projection;
	// Model View Projection Matrix
	glm::mat4 MVP;
	bool first;
};

namespace SMMath {
	/* プリミティブな関数 */

	// x 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Rx(const double deg);
	// y 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Ry(const double deg);
	// z 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Rz(const double deg);
	// 回転行列
	// 入力：x 軸回転, y 軸回転, z 軸回転
	cv::Mat1d R(const double deg_x, const double deg_y, const double deg_z);
	// x 方向への並進ベクトル
	cv::Mat1d Tx(const double l);

	/* 鏡像変換行列 */

	// 回転行列
	// 入力：法線ベクトル
	cv::Mat1d Mr(const cv::Point3d& n);

	// 平行移動ベクトル
	// 入力：法線ベクトル, 原点からの距離
	cv::Mat1d Mt(const cv::Point3d& n, const double k);

	// アフィン変換
	// 入力：法線ベクトル, 原点からの距離
	cv::Mat1d M(const cv::Point3d& n, const double k);

	/* 法線ベクトル関連 */
	// Tilt mirror の法線ベクトル
	cv::Point3d nt(const double alpha_deg, const double surf_deg, const double rtilt1_deg, const double rtilt2_deg, const double rtilt3_deg);
	// Pan mirror の法線ベクトル
	cv::Point3d np(const double beta_deg, const double surf_deg, const double rpan1_deg, const double rpan2_deg, const double rpan3_deg);

	// カメラ情報から Projection Matrix を作る
	cv::Mat1d Projection(const cv::Mat1d& camMat, const cv::Size camSz, const double znear, const double zfar);

	// 回転と平行移動を指定して 4*4 のアフィン行列を作る
	cv::Mat1d Affine(const cv::Mat1d R, const cv::Mat1d t);
}
