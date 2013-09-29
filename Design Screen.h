#ifndef DESIGN_SCREEN
#define DESIGN_SCREEN

#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>

#include <my/LineIntersection.h>

#include <vector>
#include <iostream>
#include <cmath>

#include <cpGUI/cpGUI.h>

#include "Common Properties.h"

bool PointInShape(sf::Shape& shape, sf::Vector2f point);
sf::Vector2f RotatePoint(sf::Vector2f origin, sf::Vector2f point, float angle);
void RotateShape(sf::Vector2f origin, sf::Shape& shape, float angle);
sf::Vector2f ComputeCentroid(sf::Shape& shape);
float LineAngle(sf::Vector2f line);

//returns the final body product (to be saved)
void ShowDesignScreen(sf::RenderWindow& window, cp::cpGuiContainer& gui, sf::Body<sf::Shape>& body){
	
	//only one gui can exists, so we just hide whats already in it
	const int GuiContents = gui.control.size();
	const int GuiDrawables= gui.Drawables.size();
	HideGuiContents(gui, GuiContents, GuiDrawables);

	const unsigned int BOX_HEIGHT = 20;
	const unsigned int BOX_WIDTH = 70;
	const unsigned int Y_ALIGN = 0;
	const unsigned int X_SPACE = 150;

	cp::cpTextInputBox& redInput =  *new cp::cpTextInputBox(&window, &gui, "R", X_SPACE, Y_ALIGN, BOX_WIDTH, BOX_HEIGHT);	
	cp::cpTextInputBox& greenInput= *new cp::cpTextInputBox(&window, &gui, "G", X_SPACE*2, Y_ALIGN, BOX_WIDTH, BOX_HEIGHT); 
	cp::cpTextInputBox& blueInput = *new cp::cpTextInputBox(&window, &gui, "B", X_SPACE*3, Y_ALIGN, BOX_WIDTH, BOX_HEIGHT);

	cp::cpButton& DoneBtn = *new cp::cpButton(&window, &gui, "Done", 750,0, 50, 60); 
	DoneBtn.SetLabelText("Done");
	DoneBtn.SetBackgroundColor(sf::Color(100,200,100));
	//TODO: change coloring on both?

	sf::Shape* selectedShape = 0;
	const sf::Input&  input = window.GetInput();
	sf::Vector2f clickMousePos(0,0);
	float clickRot = 0;
	bool shapeGrabbed = false;

	for(bool Done= false; Done != true;){

		sf::Event Event;
		while(window.GetEvent(Event)){

			if( Event.Type == sf::Event::MouseButtonPressed )
				for(int I= 0; I< body.GetNumbObjects(); I++)
					if( PointInShape( *body.GetObject(I), body.TransformToLocal(window.ConvertCoords(input.GetMouseX(), input.GetMouseY(), &window.GetView())) ) ){
						if(selectedShape != 0) 
							selectedShape->SetOutlineWidth(0);
						selectedShape = body.GetObject(I);
			
						clickMousePos = sf::Vector2f(input.GetMouseX(), input.GetMouseY());
						shapeGrabbed = true;
					}

			if( Event.Type == sf::Event::MouseButtonReleased ){
				for(int I= 0; I< body.GetNumbObjects(); I++)
					if( PointInShape( *body.GetObject(I), body.TransformToLocal(window.ConvertCoords(input.GetMouseX(), input.GetMouseY(), &window.GetView())) ) ){
						if(selectedShape != 0) 
							selectedShape->SetOutlineWidth(0);
						selectedShape = body.GetObject(I);
						selectedShape->SetOutlineWidth(3);
					}
				shapeGrabbed = false;
			}

			if(selectedShape != 0 && Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::LShift){
				sf::Vector2f centroid = ComputeCentroid(*selectedShape);
				sf::Vector2f line = window.ConvertCoords(input.GetMouseX(), input.GetMouseY(), &window.GetView()) - body.TransformToGlobal(selectedShape->TransformToGlobal(centroid));
				clickRot = LineAngle(line);
			}

			gui.ProcessEvent(&Event);
		}

		if( shapeGrabbed == true && input.IsKeyDown(sf::Key::LShift) == false ){
			selectedShape->Move(sf::Vector2f(input.GetMouseX(), input.GetMouseY()) - clickMousePos);
			clickMousePos.x = input.GetMouseX();
			clickMousePos.y = input.GetMouseY();
		}

		if( shapeGrabbed == true && input.IsKeyDown(sf::Key::LShift) == true ){
			sf::Vector2f centroid = ComputeCentroid(*selectedShape);

			sf::Vector2f line = window.ConvertCoords(input.GetMouseX(), input.GetMouseY(), &window.GetView()) - body.TransformToGlobal(selectedShape->TransformToGlobal(centroid));
			float newClickRot = LineAngle(line);
			float rotation = newClickRot - clickRot;
			//rotation = selectedShape->GetRotation() + rotation;

			if(rotation != 0)
				RotateShape(centroid, *selectedShape, -rotation); //TODO: find out why this needs to be negative

			clickRot = newClickRot;
		}

		if( DoneBtn.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED )
			Done= true;

		gui.CheckState(&input);

		//rendering
		window.Clear(sf::Color(210,210,255));

		gui.Draw(window);
		window.Draw(body);

		window.Display();

	}

	//retore previous GUI
	ShowGuiContents(gui, 3, 1);
		redInput.Show(false);
		greenInput.Show(false);
		blueInput.Show(false);
		DoneBtn.Show(false);
}

bool PointInShape(sf::Shape& shape, sf::Vector2f point){
	unsigned int crossCount = 0;

	for(int I= -1; I < (int)shape.GetNbPoints() -1; ){
		
		sf::Vector2f p1;
		if( I < 0 ) 
			p1 = shape.TransformToGlobal(shape.GetPointPosition( shape.GetNbPoints() + I ));
		else
			p1 = shape.TransformToGlobal(shape.GetPointPosition( I ));
		I++;
		sf::Vector2f p2 = shape.TransformToGlobal(shape.GetPointPosition(I));
																//todo: find a way to provide infinity argument
		if( lines_intersect(p1, p2, point, point + sf::Vector2f(9999999, 0)) == DO_INTERSECT )
			crossCount++;
		//TODO: handle if intersection occurs ON the actual point
	}

	if( crossCount % 2 == 0 ) return false;
	else					  return true;
}

sf::Vector2f RotatePoint(sf::Vector2f origin, sf::Vector2f point, float angle) {   
	float s = sin(ConvToRad(angle));   
	float c = cos(ConvToRad(angle));  

	// translate point back to origin:  
	point.x -= origin.x;   
	point.y -= origin.y;   

	// rotate point   
	float xnew = point.x * c - point.y * s;   
	float ynew = point.x * s + point.y * c; 
	
	// translate point back to global coords:
	sf::Vector2f TranslatedPoint;
	TranslatedPoint.x = xnew + origin.x;  
	TranslatedPoint.y = ynew + origin.y; 

	return TranslatedPoint;
} 

void RotateShape(sf::Vector2f origin, sf::Shape& shape, float angle){
	for(int I= 0; I< shape.GetNbPoints(); I++)
		shape.SetPointPosition(I, RotatePoint(origin, shape.GetPointPosition(I), angle));
}

sf::Vector2f ComputeCentroid(sf::Shape& shape) 
{ 
	sf::Vector2f centroid(0, 0); 
    double signedArea = 0.0; 
    double x0 = 0.0; // Current vertex X 
    double y0 = 0.0; // Current vertex Y 
    double x1 = 0.0; // Next vertex X 
    double y1 = 0.0; // Next vertex Y 
    double a = 0.0;  // Partial signed area 
 
    // For all vertices except last 
    int i=0; 
	for (i=0; i<shape.GetNbPoints()-1; ++i) 
    { 
		sf::Vector2f point0(shape.GetPointPosition(i));
		sf::Vector2f point1(shape.GetPointPosition(i+1));
        x0 = point0.x; 
        y0 = point0.y; 
        x1 = point1.x; 
        y1 = point1.y; 
        a = x0*y1 - x1*y0; 
        signedArea += a; 
        centroid.x += (x0 + x1)*a; 
        centroid.y += (y0 + y1)*a; 
    } 
 
    // Do last vertex 
	sf::Vector2f point0(shape.GetPointPosition(i));
	sf::Vector2f point1(shape.GetPointPosition(0));
    x0 = point0.x; 
    y0 = point0.y; 
    x1 = point1.x; 
    y1 = point1.y; 

    a = x0*y1 - x1*y0; 
    signedArea += a; 
    centroid.x += (x0 + x1)*a; 
    centroid.y += (y0 + y1)*a; 
 
    signedArea *= 0.5; 
    centroid.x /= (6*signedArea); 
    centroid.y /= (6*signedArea); 
 
    return centroid; 
} 

float LineAngle(sf::Vector2f line){

	float angle = ConvToDeg(std::atan2(line.x, line.y));
	int intAngle = angle; //modulus can only happen on ints
	angle -= intAngle;    //decimal saved and restored later

	intAngle -= 90; //atan considers vertical line 0 degrees
	intAngle += 360; //make a negative angle positive
	intAngle %= 360; //make sure angle is under 360
	
	angle+= intAngle;

	return angle;
}

#endif