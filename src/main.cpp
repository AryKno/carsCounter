#include "carsCounter.hpp"



using namespace cv;
using namespace std;

Mat image;
Mat frame;
Mat fgMaskMOG2;
Mat window2x2;
Mat window3x3;


RNG rng(12345);




int i = 0;
Ptr<BackgroundSubtractor> bgSubtractor;
int keyboard;


int nbrVehicles = 0;

int main(int, char**)
{

    list<tuple<int,int,bool>> vehiclesList;
    vehiclesListDump(&vehiclesList);
    //we open the video
    VideoCapture video;

    /*if(!video.isOpened()){ 
    //error in opening the video input
    cerr << "Unable to open file : " << VIDEO << endl<<"Check the path."<<endl;
    exit(EXIT_FAILURE);
    }*/

    bgSubtractor = createBackgroundSubtractorKNN(600,4000,false);//900 1000 false
                                                                //600 4000 false
    

    video.open(VIDEO);
    if(!video.isOpened()){ 
       //error in opening the video input
        cerr << "Unable to open file : " << VIDEO << endl<<"Check the path."<<endl;
        exit(EXIT_FAILURE);
    }

    int iterationCounter = 0;
    while((char)keyboard != 27 ){
        iterationCounter++;
        //read the current frame
        if(!video.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        frame.copyTo(image);


        //############## BACKGROUND SUBTRACTION ###################

              

        bgSubtractor->apply(frame, fgMaskMOG2);



        //############## Filtering the mask to remove noise and amplify ############################
        window2x2 = getStructuringElement(MORPH_ELLIPSE,Size(2,2)); //we create the window
        window3x3 = getStructuringElement(MORPH_RECT,Size(3,3)); //we create the window
        
        //we apply an opening to the mask to remove the noise of 1 or 2 pixels, as well as not eroding the car
        morphologyEx(fgMaskMOG2,fgMaskMOG2,MORPH_OPEN,window2x2,Point(-1,-1),1);
        //we amplify the cars
        dilate(fgMaskMOG2,fgMaskMOG2,window3x3,Point(-1,-1),6);
        

        //###################### FIND CONTOUR ############################################

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        //We look for some contours
        findContours(fgMaskMOG2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_L1 , Point(0, 0) );  //CHAIN_APPROX_TC89_L1

        
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size() );

        //for each contour found, we approximate its function and we extrapolate a bounding rectangle
        for(size_t i = 0; i<contours.size();i++)
        {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 15, true );   //10 parfait
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        //we draw the line that separate counted cars from not-counted cars
        int offset = 20;
        int lineH = 360; 
        Point p1(1,lineH);
        Point p2(1280,lineH);


        line(fgMaskMOG2, p1, p2, Scalar(255,0,0), 1);
        line(image, p1, p2, Scalar(255,0,0), 3);

        //the content of the list of tracked vehicles
        cout << "Vehicles list"<<endl;
        vehiclesListDump(&vehiclesList);

        //we remove all cars that has been counted
        vehiclesAfterLimit(&vehiclesList);
        
        
        //for each extrapolated contours
        for( size_t i = 0; i<contours.size(); i++ )
        {
            //if the contours has a size close to a car (to avoid detecting any noise that could have survived through the filtering step)
            if(boundRectIsValid(&boundRect[i],VHCL_MIN_WIDTH,VHCL_MIN_HEIGHT))
            {
                //we draw the contour to see how our filtering operation is efficient
                drawContours( image, contours_poly, (int)i, Scalar(0,0,255), 1, 8, vector<Vec4i>(), 0, Point() );

                //for each contour (or boundRect, every car has a contour and a bounding rectangle) that is below the the counting line and a little bit above
                if(boundRect[i].y >= lineH-offset)
                {
                    //we track the car
                    //this function track every contour position and decide if a new car appear, a car has moved, a car has crossed the line
                    trackCars(&vehiclesList,boundRect[i].x,boundRect[i].y,VLIST_AREAX,VLIST_AREAY,lineH,&nbrVehicles);
                }

                if(boundRect[i].y >= lineH)//If the car is below the line
                {
                    //we draw a green bounding rectangle for cars not counted
                    circle(image,Point(boundRect[i].x,boundRect[i].y),10,Scalar(0,255,0),5);
                    rectangle(image,Point(boundRect[i].x,boundRect[i].y),Point(boundRect[i].x+boundRect[i].width,boundRect[i].y+boundRect[i].height),Scalar(0,255,0),2);
                }
                else
                {
                    //we draw a red one for the counted ones
                    rectangle(image,Point(boundRect[i].x,boundRect[i].y),Point(boundRect[i].x+boundRect[i].width,boundRect[i].y+boundRect[i].height),Scalar(0,0,255),2);
                }
                                    
                stringstream pos;
                pos << "(" << boundRect[i].x << "," << boundRect[i].y << ")";
                writeText(&image,pos.str(),boundRect[i].x,boundRect[i].y,Scalar(255,255,0),0.7);

                stringstream counterStr;
                counterStr << nbrVehicles;
                writeText(&image,counterStr.str(),50,50,Scalar(0,255,0),2);
                circle(image,Point(boundRect[i].x,boundRect[i].y),10,Scalar(0,0,255),5);
            }
        }

      //##########################We save the masks and the images###########################

        stringstream fn;
        fn<<"./images/samples/Image"<<i<<".jpg";
            imwrite(fn.str(), image );
        stringstream fn2;
        fn2<<"./images/samples/Image_mask_"<<i<<".jpg";
            imwrite(fn2.str(), fgMaskMOG2 );
        keyboard = waitKey( 30 );
        i++;

        image.release();
        frame.release();
        fgMaskMOG2.release();
    }
    video.release();
    destroyAllWindows();
    return EXIT_SUCCESS;
}

