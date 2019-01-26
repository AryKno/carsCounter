#include "carsCounter.hpp"



using namespace cv;
using namespace std;

bool boundRectIsValid(Rect *rect,int minWidth,int minHeight)
{
    return (rect->width>minWidth && rect->height>minHeight);
}

void writeText(Mat *frame, string s, int x, int y, Scalar color, double size)
{   //write text at the postion x,y
    putText(*frame, s.c_str(), Point(x,y),FONT_HERSHEY_SIMPLEX, size , color);//display the number
    //show the current frame and the fg masks
}

void trackCars(list<tuple<int,int,bool>> *vehiclesList, int x, int y, int Xwidth, int Ywidth, int yLimit, int *counter)
{   //This function save or update each cars position in a list of tuples (x,y,isCounted).
    //The car position is updated at each call, a new car is added only if the position (x,y) y > yLimit (aka under the count line)

    bool newOne = true;

    for(list<tuple<int,int,bool>>::iterator it = vehiclesList->begin(); it != vehiclesList->end();++it)
    {
        int _x = get<0>(*it);
        int _y = get<1>(*it);
        int leftX = _x - Xwidth;
        int rightX = _x + Xwidth;
        int topY = _y + Ywidth;
        int bottomY =_y - Ywidth;

        if(x <= rightX && x >= leftX && y <= topY && y >= bottomY)
        {   //If the point (x,y) is in the area of the point i, then it's the same vehicle and we update it's value
            get<0>(*it) = x;
            get<1>(*it) = y;
            newOne = false;
            
            if(get<2>(*it) == false && get<1>(*it) < yLimit) //If the car pass the limit, we count it
            {
                *counter = *counter + 1;
                get<2>(*it) = true;
            }
            break;
        }
    }

    if(newOne && x>0 && y>0 && y >= yLimit) //it's a new one, we add it to the list
    {
        tuple<int,int,bool> t(x,y,false);
        vehiclesList->push_back(t); //we had the element to the vehicles list
    }
}

int vehiclesAfterLimit(list<tuple<int,int,bool>> *vehiclesList)
{   //erase all vehicles from the list if it has crossed the limit
    int count = 0;
    list<tuple<int,int,bool>>::iterator it = vehiclesList->begin();

    if(vehiclesList->empty())
        return 0;

    while(it != vehiclesList->end())
    {
        if(get<2>(*it) == true)   //if the vehicles has been counted and is at a minimum distance after the count line 
        {
            cout<< "(" << get<0>(*it) << "," << get<1>(*it) << ")"<<endl;
            it = vehiclesList->erase(it);
            count++;
        }
        else
        {
            ++it; //we increment, only when an element has not been removed
        }
    }
    return count;
}



void vehiclesListDump(list<tuple<int,int,bool>> *vehiclesList)
{
    if(vehiclesList->empty())
        cout<<"empty list"<<endl;
    for(list<tuple<int,int,bool>>::iterator it = vehiclesList->begin(); it != vehiclesList->end();++it)
    {
        cout << " Vehicle : <" << get<0>(*it) << "," << get<1>(*it)<< "," << get<2>(*it) << ">"<<endl;        
    }
}

