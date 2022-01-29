#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
using namespace std;

// Used to store images
Mat imgOriginal, imgGrayscale, imgContours, imgHough, imgOutput;

void init_canny() 
{
  int seuilC = 50;
  int highThreshold = seuilC*3;
  createTrackbar("seuilC", "scan", nullptr, 100, NULL);
  setTrackbarPos( "seuilC", "scan", seuilC );
}

void init_hough() 
{
    int seuilH = 70;
    createTrackbar("seuilH", "scan", nullptr, 255, NULL);
    setTrackbarPos( "seuilH", "scan", seuilH );
}

Mat apply_canny(Mat imgInput) 
{
  Mat imgBlurred, imgOutput;
  // Blur Effect 
  GaussianBlur(imgInput,imgBlurred,Size(5, 5),1.8);            

  // Canny Image
  int lowThreshold = getTrackbarPos( "seuilC", "scan" );
  int highThreshold = lowThreshold*3; // Canny's recommendation
  Canny(imgInput,imgOutput,lowThreshold,highThreshold);
  return imgOutput;
}

Mat apply_watershed(Mat imgInput)
{
  Mat imgOutput = imgInput.clone();
  Mat imgMarkerMask(imgInput.size(), CV_32SC1);
  imgMarkerMask = Scalar::all(0);

  // top rectangle
  imgMarkerMask(Rect(0,0,imgInput.cols, 25)) = Scalar::all(1);
  // bottom rectangle
  imgMarkerMask(Rect(0,imgInput.rows-25,imgInput.cols, 25)) = Scalar::all(1);
  // left rectangle
  imgMarkerMask(Rect(0,0,25,imgInput.rows)) = Scalar::all(1);
  // right rectangle
  imgMarkerMask(Rect(imgInput.cols-25,0,25,imgInput.rows)) = Scalar::all(1);
  // center rectangle
  imgMarkerMask(Rect(imgInput.cols/2-150,imgInput.rows/2-100,300, 200)) = Scalar::all(2);

  watershed( imgInput, imgMarkerMask );
  for (int i = 0; i < imgMarkerMask.rows; i++) {
    for (int j = 0; j < imgMarkerMask.cols; j++) {
       if (imgMarkerMask.at<int>(i, j) == -1) {
         imgOutput.at<Vec3b>(i, j)[0] = 255; 
         imgOutput.at<Vec3b>(i, j)[1] = 255; 
         imgOutput.at<Vec3b>(i, j)[2] = 255;
       }
       else {
         imgOutput.at<Vec3b>(i, j)[0] = 0; 
         imgOutput.at<Vec3b>(i, j)[1] = 0; 
         imgOutput.at<Vec3b>(i, j)[2] = 0;
       }

    }
  }
  return imgOutput;
}

Vec2f averageLines(vector<Vec2f> lines) {
  Vec2f averageLine;
  float sumRho, sumThetha;
  for(int i = 0; i < lines.size(); i++)
  {
    sumRho += lines[i][0];
    sumThetha += lines[i][1];
  }
  averageLine[0] = sumRho / lines.size();
  averageLine[1] = sumThetha / lines.size();
  return averageLine;
}

Vec2f getIntersectionPoint(Vec2f line1, Vec2f line2)
{
  // line 1
  float a, b, c;
  a = cos(line1[1]);
  b = sin(line1[1]);
  c = line1[0];

  // line 2
  float a2, b2, c2;
  a2 = cos(line2[1]);
  b2 = sin(line2[1]);
  c2 = line2[0];

  // compute intersection point position (Cramer's rule)
  Vec2f point;
  point[0] = (c*b2-c2*b) / (a*b2-a2*b);
  point[1] = (a*c2-a2*c) / (a*b2-a2*b);
  return point;
}

// Contrast enhancement
Mat filtreR(Mat input, double alpha)
{
  Mat R = cv::Mat::zeros(3, 3, CV_32FC1);
  R.at<float>(0, 1) = -alpha;
  R.at<float>(1, 0) = -alpha;
  R.at<float>(1, 1) = 1.0 + 4.0*alpha;
  R.at<float>(1, 2) = -alpha;
  R.at<float>(2, 1) = -alpha;
  Mat output;
  cv::filter2D(input, output, CV_8U, R);
  return output;
}

Mat apply_hough(Mat imgInput)
{
  Mat houghOutput;
  houghOutput = imgInput.clone();
  if (imgInput.type() != 0) cvtColor(imgInput, imgInput, COLOR_BGR2GRAY);

  vector<Vec2f> lines;
  int threshold = getTrackbarPos( "seuilH", "scan" );
  HoughLines(imgInput, lines, 1, CV_PI/180, threshold, 0, 0 );

  // Draw the lines & separate into different classes
  vector<Vec2f> linesT, linesB, linesL, linesR;
  for( int i = 0; i < lines.size(); i++ )
  {
      float rho = lines[i][0], theta = lines[i][1];
      Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));

      // Top Class
      if ( pt1.y > 10 && pt1.y < 200 && pt2.y > 10 && pt2.y < 200 ) {
        linesT.push_back(lines[i]);
        line( houghOutput, pt1, pt2, Scalar(255,0,0), 2, LINE_AA);
      }
      // Bottom Class
      if ( pt1.y > 280 && pt1.y < 470 && pt2.y > 280 && pt2.y < 470 ) {
        linesB.push_back(lines[i]);
        line( houghOutput, pt1, pt2, Scalar(0,255,0), 2, LINE_AA);
      }
      // Left Class
      if ( pt1.x > 10 && pt1.x < 240 && pt2.x > 10 && pt2.x < 240 ) {
        linesL.push_back(lines[i]);
        line( houghOutput, pt1, pt2, Scalar(255,255,0), 2, LINE_AA);
      }
      // Right Class
      if ( pt1.x > 400 && pt1.x < 630 && pt2.x > 400 && pt2.x < 630 ) {
        linesR.push_back(lines[i]);
        line( houghOutput, pt1, pt2, Scalar(0,255,255), 2, LINE_AA);
      }
  }

  if (!linesT.empty() && !linesB.empty() && !linesL.empty() && !linesR.empty())
  {
    // Get average line per class
    Vec2f averageT = averageLines(linesT);
    Vec2f averageB = averageLines(linesB);
    Vec2f averageL = averageLines(linesL);
    Vec2f averageR = averageLines(linesR);

    // Get intersection points
    Vec2f pointTL = getIntersectionPoint(averageT, averageL);
    Vec2f pointTR = getIntersectionPoint(averageT, averageR);
    Vec2f pointBL = getIntersectionPoint(averageB, averageL);
    Vec2f pointBR = getIntersectionPoint(averageB, averageR);

    // Transform & warpPerspective
    float height = static_cast<float>(480);
    float width = static_cast<float>(640);
    vector<Vec2f> intersectPts{pointTL, pointTR, pointBR, pointBL};
    vector<Vec2f> dstPts{ {0,0}, {width, 0}, {width, height}, {0, height} };
    Mat warpMatrix = getPerspectiveTransform(intersectPts, dstPts);

    // Perspective transform operation using transform matrix
    Mat output;
    warpPerspective(imgOriginal, output, warpMatrix, imgOriginal.size(), INTER_LINEAR);

    output =  filtreR(output, 3);
    imshow("scanned image", output);
  }

  return houghOutput;
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <methode-extraction-contours>" << std::endl;
    std::cout << "Méthodes existantes : " << std::endl;
    std::cout << "- Canny" << std::endl;
    std::cout << "- Watershed" << std::endl;
    exit(EXIT_FAILURE);
    }
    char * extract_meth = argv[1];
    if (!(extract_meth == string("Canny") || extract_meth == string("Watershed")))
    {
      std::cout << "Enter valid extraction method name" << std::endl;
      exit(EXIT_FAILURE);
    }

    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;
    namedWindow("scan", WINDOW_NORMAL);
    namedWindow("scanned image", WINDOW_AUTOSIZE);
    resizeWindow("scan", 1200, 1300);

    // Trackbar initialisations
    init_hough();
    if (extract_meth == string("Canny")) {
      init_canny();
    }

    for(;;)
    {
        // Store video frame into image matrix
        cap >> imgOriginal;

        // Color to Grayscale
        cvtColor(imgOriginal, imgGrayscale, COLOR_BGR2GRAY);

        // Apply Extraction
        if (extract_meth == string("Canny")) {
          imgContours = apply_canny(imgGrayscale);
        }

        // Watershed
        if (extract_meth == string("Watershed")) {
          imgContours = apply_watershed(imgOriginal);
        }

        // Hough
        imgHough = apply_hough(imgContours);
        imgOutput = 0.25* imgHough + 0.75* imgOriginal;
        // Draw scan region rectangle
        Rect r5 = Rect(imgOutput.cols/2-250,imgOutput.rows/2-175,500, 350);
        rectangle(imgOutput,r5,Scalar(0,0,255),2,8,0);

        //Display image
        imshow("scan", imgOutput);

        int   key_code = waitKey(30);
        int ascii_code = key_code & 0xff; 
        if( ascii_code == 'q') break;
    }
    return 0;
}