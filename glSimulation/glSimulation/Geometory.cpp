#include "Geometory.h"
#include <opencv2/opencv.hpp>
#include <cmath>

namespace SMMath {
	/* プリミティブな関数 */

	// x 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Rx(const double deg) {
		const double rad = deg / 180.0 * CV_PI;
		const double c = cos(rad);
		const double s = sin(rad);
		return (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, c, -s, 0, s, c);
	}
	// y 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Ry(const double deg) {
		const double rad = deg / 180.0 * CV_PI;
		const double c = cos(rad);
		const double s = sin(rad);
		return (cv::Mat_<double>(3, 3) << c, 0, s, 0, 1, 0, -s, 0, c);
	}
	// z 軸まわりの回転
	// 入力：回転角
	cv::Mat1d Rz(const double deg) {
		const double rad = deg / 180.0 * CV_PI;
		const double c = cos(rad);
		const double s = sin(rad);
		return (cv::Mat_<double>(3, 3) << c, -s, 0, s, c, 0, 0, 0, 1);
	}

	// 回転行列
	// 入力：x 軸回転, y 軸回転, z 軸回転
	cv::Mat1d R(const double deg_x, const double deg_y, const double deg_z) {
		return Rz(deg_z) * Ry(deg_y) * Rx(deg_x);
	}
	// x 方向への並進ベクトル
	cv::Mat1d Tx(const double l) {
		cv::Mat1d ret = cv::Mat::eye(4, 4, CV_64FC1);
		ret(0, 3) = l;
		return ret;
	}

	/* 鏡像変換行列 */

	// 回転行列
	// 入力：法線ベクトル
	cv::Mat1d Mr(const cv::Point3d& n) {
		return (cv::Mat_<double>(3, 3) <<
			1.0 - 2.0 * n.x * n.x, -2.0 * n.x * n.y, -2.0 * n.x * n.z,
			-2.0 * n.x * n.y, 1.0 - 2.0 * n.y * n.y, -2.0 * n.y * n.z,
			-2.0 * n.x * n.z, -2.0 * n.y * n.z, 1.0 - 2.0 * n.z * n.z);
	}

	// 平行移動ベクトル
	// 入力：法線ベクトル, 原点からの距離
	cv::Mat1d Mt(const cv::Point3d& n, const double k) {
		return (cv::Mat_<double>(3, 1) << 2.0 * k * n.x, 2.0 * k * n.y, 2.0 * k * n.z );
	}

	// アフィン変換
	// 入力：法線ベクトル, 原点からの距離
	cv::Mat1d M(const cv::Point3d& n, const double k) {
		cv::Mat1d ret = cv::Mat::eye(4, 4, CV_64FC1);
		Mr(n).copyTo(ret(cv::Rect(0, 0, 3, 3)));
		Mt(n, k).copyTo(ret(cv::Rect(3, 0, 1, 3)));
		return ret;
	}

	/* 法線ベクトル関連 */
	// Tilt mirror の法線ベクトル
	cv::Point3d nt(const double alpha_deg, const double surf_deg, const double rtilt1_deg, const double rtilt2_deg, const double rtilt3_deg) {
		const cv::Mat1d e_y = (cv::Mat_<double>(3, 1) << 0, -1, 0);
		const cv::Mat1d Rsurf = Rx(surf_deg);
		const cv::Mat1d Ralpha = Rz(alpha_deg);
		const cv::Mat1d Rtilt_x = Rx(rtilt1_deg);
		const cv::Mat1d Rtilt_y = Ry(rtilt2_deg);
		const cv::Mat1d Rtilt_z = Rz(rtilt3_deg);

		const cv::Mat1d n_tilt = Rtilt_z * Rtilt_y * Rtilt_x * Ralpha * Rsurf * e_y;
		const auto sqr = [](const double x) {return x * x; };
		const double scale = std::sqrt(sqr(n_tilt(0)) + sqr(n_tilt(1)) + sqr(n_tilt(2)));

		return cv::Point3d(n_tilt(0) / scale, n_tilt(1) / scale, n_tilt(2) / scale);
	}
	// Pan mirror の法線ベクトル
	cv::Point3d np(const double beta_deg, const double surf_deg, const double rpan1_deg, const double rpan2_deg, const double rpan3_deg) {
		const cv::Mat1d e_x = (cv::Mat_<double>(3, 1) << -1, 0, 0);
		const cv::Mat1d Rsurf = Rz(surf_deg);
		const cv::Mat1d Rbeta = Ry(beta_deg);
		const cv::Mat1d Rpan_x = Rx(rpan1_deg);
		const cv::Mat1d Rpan_y = Ry(rpan2_deg);
		const cv::Mat1d Rpan_z = Rz(rpan3_deg);

		const cv::Mat1d n_pan = Rpan_z * Rpan_y * Rpan_x * Rbeta * Rsurf * e_x;
		const auto sqr = [](const double x) {return x * x; };
		const double scale = std::sqrt(sqr(n_pan(0)) + sqr(n_pan(1)) + sqr(n_pan(2)));

		return cv::Point3d(n_pan(0) / scale, n_pan(1) / scale, n_pan(2) / scale);
	}

	// カメラ情報から Projection Matrix を作る
	cv::Mat1d Projection(const cv::Mat1d& camMat, const cv::Size camSz, const double znear, const double zfar) {
		const double fx = camMat(0, 0);
		const double fy = camMat(1, 1);
		const double cx = camMat(0, 2);
		const double cy = camMat(1, 2);
		const double w = camSz.width, h = camSz.height;

		//const cv::Mat1d ret = (cv::Mat_<double>(4, 4) <<
		//	2.0 * fx / w, 0, 0, 0,
		//	0, 2.0 * fy / w, 0, 0,
		//	1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear), -1.0,
		//	0, 0, -2.0 * zfar * znear / (zfar - znear), 0
		//	);
		const cv::Mat1d ret = (cv::Mat_<double>(4, 4) <<
			2.0 * fx / w, 0, (w - 2 * cx) / w, 0,
			0, 2.0 * fy / w, -(h - 2 * cy) / h, 0,
			0, 0, -(zfar + znear) / (zfar - znear), -(2 * zfar * znear) / (zfar - znear),
			0, 0, -1, 0
			);
		return ret;
	}

	// 回転と平行移動を指定して 4*4 のアフィン行列を作る
	cv::Mat1d Affine(const cv::Mat1d R, const cv::Mat1d t) {
		cv::Mat1d ret = cv::Mat::eye(4, 4, CV_64FC1);
		R.copyTo(ret(cv::Rect(0, 0, 3, 3)));
		t.copyTo(ret(cv::Rect(3, 0, 1, 3)));
		return ret;
	}
}

Geometory::Geometory(const cv::Mat1d& Tp, const cv::Mat1d& Rc, const cv::Mat1d& K, const cv::Size camSz, const float L) 
	: L(L)
	, alpha0(45.0f)
	, beta0(45.0f)
	, tilt_scale(1.0f)
	, pan_scale(1.0f)
	, gamma_t(0.0f)
	, gamma_p(0.0f)
	, r_tilt(0.0f)
	, Tp(Tp)
	, Rc(Rc)
	, Projection(SMMath::Projection(K, camSz, 0.01, 5000))
	, d(0.0f)
	, first(false)
{}
	
const glm::mat4 Geometory::getMVP() const {
	return MVP;
}

void Geometory::update(const float alpha, const float beta) {
	const double alpha_deg = tilt_scale * alpha + alpha0;
	const double beta_deg = pan_scale * beta + beta0;

	const cv::Point3d nt = SMMath::nt(alpha_deg, 0, 0, 0, 0);
	const cv::Point3d np = SMMath::np(beta_deg, 0, 0, 0, 0);

	const cv::Mat1d Rmi = Rc * SMMath::Mr(nt) * SMMath::Mr(np);
	const cv::Mat1d Ci = SMMath::M(np, d) * SMMath::Tx(-L) * SMMath::M(nt, d) * SMMath::Tx(L);
	const cv::Mat1d Tpi = Ci * (cv::Mat_<double>(4, 1) << Tp(0), Tp(1), Tp(2), 1.0);
	const cv::Mat1d tpi = (cv::Mat_<double>(3, 1) << Tpi(0), Tpi(1), Tpi(2));
	const cv::Mat1d Rmi_tpi = Rmi * tpi;
	const cv::Mat1d Rtmi = SMMath::Affine(Rmi, -Rmi_tpi);
	const cv::Mat1d Pmi = Projection * Rtmi;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			MVP[i][j] = Pmi(j, i);
		}
	}
	if (0) {
		std::cout << "nt " << nt << std::endl;
		std::cout << "np " << np << std::endl;
		std::cout << "Rtmi " << Rtmi << std::endl;
		first = true;
	}
}