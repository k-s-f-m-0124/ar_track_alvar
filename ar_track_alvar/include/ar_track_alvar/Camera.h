/*
 * This file is part of ALVAR, A Library for Virtual and Augmented Reality.
 *
 * Copyright 2007-2012 VTT Technical Research Centre of Finland
 *
 * Contact: VTT Augmented Reality Team <alvar.info@vtt.fi>
 *          <http://www.vtt.fi/multimedia/alvar.html>
 *
 * ALVAR is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALVAR; if not, see
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>.
 */

#ifndef CAMERA_H
#define CAMERA_H

/**
 * \file Camera.h
 *
 * \brief This file implements a camera used for projecting points and
 * computing homographies.
 */

#include "Alvar.h"
#include "Pose.h"
#include "Util.h"
#include "FileFormat.h"
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <image_transport/image_transport.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <resource_retriever/retriever.hpp>

namespace alvar
{
/** \brief Simple structure for collecting 2D and 3D points e.g. for camera
 * calibration */
struct ALVAR_EXPORT ProjPoints
{
  int width;
  int height;

  /** \brief 3D object points corresponding with the detected 2D image points.
   */
  std::vector<cv::Point3d> object_points;
  /** \brief Detected 2D object points
   * If point_counts[0] == 10, then the
	 * If point_counts[0] == 10, then the 
   * If point_counts[0] == 10, then the
	 * If point_counts[0] == 10, then the 
   * If point_counts[0] == 10, then the
   * first 10 points are detected in the first frame. If
	 * first 10 points are detected in the first frame. If 
   * first 10 points are detected in the first frame. If
	 * first 10 points are detected in the first frame. If 
   * first 10 points are detected in the first frame. If
   * point_counts[1]	== 6, then the next 6 of these points are
	 * point_counts[1]	== 6, then the next 6 of these points are 
   * point_counts[1]	== 6, then the next 6 of these points are
	 * point_counts[1]	== 6, then the next 6 of these points are 
   * point_counts[1]	== 6, then the next 6 of these points are
   * detected in the next frame... etc.
   */
  std::vector<cv::Point2d> image_points;
  /** \brief Vector indicating how many points are detected for each frame */
  std::vector<int> point_counts;

  /** \brief Reset \e object_points , \e  image_points and \e point_counts */
  void Reset();

  /** \brief Add elements to \e object_points , \e  image_points and \e
   * point_counts using Chessboard pattern */
  bool AddPointsUsingChessboard(const cv::Mat& image, double etalon_square_size,
                                int etalon_rows, int etalon_columns,
                                bool visualize);

  /** \brief Add elements to \e object_points , \e  image_points and \e
   * point_counts using detected markers */
  bool AddPointsUsingMarkers(std::vector<PointDouble>& marker_corners,
                             std::vector<PointDouble>& marker_corners_img,
                             cv::Mat& image);
};

/**
 * \brief Simple \e Camera class for calculating distortions, orientation or
 * projections with pre-calibrated camera
 */
class ALVAR_EXPORT Camera
{
public:
  cv::Mat calib_K;
  double calib_K_data[3][3];
  cv::Mat calib_D;
  double calib_D_data[4];
  int calib_x_res;
  int calib_y_res;
  int x_res;
  int y_res;
  bool getCamInfo_;

protected:
  std::string cameraInfoTopic_;
	sensor_msgs::msg::CameraInfo cam_info_;
	// void camInfoCallback (const sensor_msgs::msg::CameraInfo::SharedPtr cam_info);
	// rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr sub_;

private:
  bool LoadCalibXML(const char* calibfile);
  bool LoadCalibOpenCV(const char* calibfile);
  bool SaveCalibXML(const char* calibfile);
  bool SaveCalibOpenCV(const char* calibfile);

public:
  /** \brief One of the two methods to make this class serializable by \e
   * Serialization class */
  std::string SerializeId()
  {
    return "camera";
  };
  /** \brief One of the two methods to make this class serializable by \e
   * Serialization class
   *
   * You can serialize the \e Camera class using filename or any std::iostream
	 * You can serialize the \e Camera class using filename or any std::iostream 
   * You can serialize the \e Camera class using filename or any std::iostream
	 * You can serialize the \e Camera class using filename or any std::iostream 
   * You can serialize the \e Camera class using filename or any std::iostream
   * as follows:
   * \code
   * alvar::Camera cam;
   * cam.SetCalib("calib.xml", 320, 240);
   * Serialization sero("calib1.xml");
   * sero<<cam;
   * \endcode
   * \code
   * alvar::Camera cam;
   * Serialization seri("calib1.xml");
   * seri>>cam;
   * cam.SetRes(320, 240);
   * \endcode
   * \code
   * std::stringstream ss;
   * Serialization sero(ss);
   * sero<<cam;
   * std::cout<<ss.str()<<std::endl;
   * // ...
   * Serialization seri(ss);
   * seri>>cam;
   * \endcode
   */
  bool Serialize(Serialization* ser)
  {
    if (!ser->Serialize(calib_x_res, "width"))
      return false;
    if (!ser->Serialize(calib_y_res, "height"))
      return false;
    if (!ser->Serialize(calib_K, "intrinsic_matrix"))
      return false;
    if (!ser->Serialize(calib_D, "distortion"))
      return false;
    return true;
  }

  /** \brief Constructor */
  Camera();
  // Camera(ros::NodeHandle& n, std::string cam_info_topic);

  /** Sets the intrinsic calibration */
  void SetCameraInfo(const sensor_msgs::msg::CameraInfo::SharedPtr cam_info);


  /** \brief Get x-direction FOV in radians */
  double GetFovX()
  {
    return (2.0f * atan2(double(x_res) / 2.0f, (double)calib_K_data[0][0]));
		return (2.0f * atan2(double(x_res) / 2.0f, (double)calib_K_data[0][0]));	
    return (2.0f * atan2(double(x_res) / 2.0f, (double)calib_K_data[0][0]));
		return (2.0f * atan2(double(x_res) / 2.0f, (double)calib_K_data[0][0]));	
    return (2.0f * atan2(double(x_res) / 2.0f, (double)calib_K_data[0][0]));
  }
  /** \brief Get y-direction FOV in radians */
  double GetFovY()
  {
    return (2.0f * atan2(double(y_res) / 2.0f, (double)calib_K_data[1][1]));
  }

  void SetSimpleCalib(int _x_res, int _y_res, double f_fac = 1.);

  /** \brief Set the calibration file and the current resolution for which the
   * calibration is adjusted to \param calibfile File to load. \param _x_res
   * Width of images captured from the real camera. \param _y_res Height of
   * images captured from the real camera. \param format FILE_FORMAT_OPENCV
   * (default) or FILE_FORMAT_XML (see doc/Camera.xsd).
   */
  bool SetCalib(const char* calibfile, int _x_res, int _y_res,
                FILE_FORMAT format = FILE_FORMAT_DEFAULT);

  /** \brief Save the current calibration information to a file
   * \param calibfile File to save.
   * \param format FILE_FORMAT_OPENCV (default) or FILE_FORMAT_XML (see
   * doc/Camera.xsd).
   */
  bool SaveCalib(const char* calibfile,
                 FILE_FORMAT format = FILE_FORMAT_DEFAULT);

  /** \brief Calibrate using the collected \e ProjPoints */
  void Calibrate(ProjPoints& pp);

  /** \brief If we have no calibration file we can still adjust the default
   * calibration to current resolution  */
  void SetRes(int _x_res, int _y_res);

  /** \brief Get OpenGL matrix
	/** \brief Get OpenGL matrix 
  /** \brief Get OpenGL matrix
	/** \brief Get OpenGL matrix 
  /** \brief Get OpenGL matrix
   * Generates the OpenGL projection matrix based on OpenCV intrinsic camera
   * matrix K. \code 2*K[0][0]/width  2*K[0][1]/width   -(2*K[0][2]/width+1)  0
   *     0                2*K[1][1]/height  2*K[1][2]/height-1    0
   *     0                0                 -(f+n)/(f-n)          -2*f*n/(f-n)
   *     0                0                 -1                    0
   * \endcode
   *
   * Note, that the sign of the element [2][0] is changed. It should be
   * \code
   *     2*K[0][2]/width+1
   * \endcode
   *
   * The sign change is due to the fact that with OpenCV and OpenGL projection
	 * The sign change is due to the fact that with OpenCV and OpenGL projection 
   * The sign change is due to the fact that with OpenCV and OpenGL projection
	 * The sign change is due to the fact that with OpenCV and OpenGL projection 
   * The sign change is due to the fact that with OpenCV and OpenGL projection
   * matrices both y and z should be mirrored. With other matrix elements
   * the sign changes eliminate each other, but with principal point
   * in x-direction we need to make the change by hand.
   */
  void GetOpenglProjectionMatrix(double proj_matrix[16], const int width,
                                 const int height,
                                 const float far_clip = 1000.0f,
                                 const float near_clip = 0.1f);

  /** \brief Invert operation for \e GetOpenglProjectionMatrix */
  void SetOpenglProjectionMatrix(double proj_matrix[16], const int width,
                                 const int height);

  /** \brief Unapplys the lens distortion for points on image plane. */
  void Undistort(std::vector<PointDouble>& points);

  /** \brief Unapplys the lens distortion for one point on an image plane. */
  void Undistort(PointDouble& point);

  /** \brief Unapplys the lens distortion for one point on an image plane. */
  void Undistort(cv::Point2f& point);

  /** \brief Applys the lens distortion for one point on an image plane. */
  void Distort(cv::Point2f& point);

  /** \brief Applys the lens distortion for points on image plane. */
  void Distort(std::vector<PointDouble>& points);

  /** \brief Applys the lens distortion for points on image plane. */
  void Distort(PointDouble& point);

  /** \brief Calculate exterior orientation
   */
  void CalcExteriorOrientation(const std::vector<cv::Point3d>& pw,
                               const std::vector<PointDouble>& pi,
                               cv::Mat& rodriques, cv::Mat& tra) const;

  /** \brief Calculate exterior orientation
   */
  void CalcExteriorOrientation(const std::vector<PointDouble>& pw,
                               const std::vector<PointDouble>& pi,
                               cv::Mat& rodriques, cv::Mat& tra) const;

  /** \brief Calculate exterior orientation
   */
  void CalcExteriorOrientation(const std::vector<PointDouble>& pw,
                               const std::vector<PointDouble>& pi,
                               Pose* pose) const;

  /** \brief Project one point */
  void ProjectPoint(const cv::Point3d& pw, const Pose* pose,
                    cv::Point2d& pi) const;

  /** \brief Project one point */
  void ProjectPoint(const cv::Point3f& pw, const Pose* pose,
                    cv::Point2f& pi) const;

  /** \brief Project points */
  void ProjectPoints(std::vector<cv::Point3d>& pw, Pose* pose,
                     std::vector<cv::Point2d>& pi) const;

  /** \brief Project points
   */
  void ProjectPoints(const cv::Mat& object_points,
                     const cv::Mat& rotation_vector,
                     const cv::Mat& translation_vector,
                     cv::Mat& image_points) const;

  /** \brief Project points
   */
  void ProjectPoints(const cv::Mat& object_points, double gl[16],
                     cv::Mat& image_points) const;

  /** \brief Project points  */
  void ProjectPoints(const cv::Mat& object_points, const Pose* pose,
                     cv::Mat& image_points) const;
};

/**
 * \brief Simple \e Homography class for finding and projecting points between
 * two planes.
 */
struct ALVAR_EXPORT Homography
{
  cv::Mat H;

  /** \brief Constructor  */
  Homography();

  /** \brief Find Homography for two point-sets */
  void Find(const std::vector<PointDouble>& pw,
            const std::vector<PointDouble>& pi);

  /** \brief Project points using the Homography */
  void ProjectPoints(const std::vector<PointDouble>& from,
                     std::vector<PointDouble>& to) const;
};

}  // namespace alvar

#endif
