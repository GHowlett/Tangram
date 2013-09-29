#include <Box2D\Box2D.h>

#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>

#include <my/LineIntersection.h>
#include <my/SFboxML.h> 

#include <vector>
#include <iostream>

#include <cpGUI/cpGUI.h>

#include "Common Properties.h"

bool CutBody(sf::Body<sf::Shape>& body, sf::Vector2f p1, sf::Vector2f p2);
void CreateRandColor(sf::Color& color);

//final body product returned to be saved
void ShowCutScreen(sf::RenderWindow& window, cp::cpGuiContainer& gui, sf::Body<sf::Shape>& body){
	
	//only one gui can exists, so we just hide whats already in it
	const int GuiContents = gui.control.size();
	const int GuiDrawables= gui.Drawables.size();
	HideGuiContents(gui, GuiContents, GuiDrawables);

	//Box2D stuff
	b2World world(b2Vec2(0, 0), true); 
	my::BodyRep bodyRep;

	b2BodyDef   BorderDef;
		BorderDef.position = b2Vec2(0,0);
		//BorderDef.type = ?;
		b2Body* borderBody = world.CreateBody(&BorderDef);

		b2FixtureDef topBorder;
			topBorder.density = 0;
			topBorder.restitution = .8;
			b2PolygonShape topShape;
				topShape.SetAsBox((400.f /30.f), (25.f /30.f), b2Vec2((400.f /30.f), (-25.f /30.f)), 0);
				topBorder.shape= &topShape;
		borderBody->CreateFixture(&topBorder);

		b2FixtureDef bottomBorder;
			bottomBorder.density = 0;
			bottomBorder.restitution = .8;
			b2PolygonShape bottomShape;
				bottomShape.SetAsBox((400.f /30.f), (25.f /30.f), b2Vec2((400.f /30.f), (625.f /30.f)), 0);
				bottomBorder.shape= &bottomShape;
		borderBody->CreateFixture(&bottomBorder);

	//GUI
	cp::cpTextBox& Instructions = *new cp::cpTextBox(&window, &gui, 0,0, 750, 60);
	Instructions.SetFontSize(20);
	Instructions.SetText("Cut up the body into whatever pieces you want, and click ""done"" when finished. \nMake sure the cut extends across the entire shape");

	cp::cpButton& DoneBtn = *new cp::cpButton(&window, &gui, "Done", 750,0, 50, 60); 
	DoneBtn.SetLabelText("Done");
	DoneBtn.SetBackgroundColor(sf::Color(100,200,100));
	//TODO: change coloring on both?

	body.SetPosition(400, 585);
	if(body.GetNumbObjects() < 2) body.SetFillColor(sf::Color(255, 75, 75));

	Line Line;

	const sf::Input& Input = window.GetInput();

	for(bool Done= false; Done!= true && window.IsOpened();){
		sf::Event Event;
		while(window.GetEvent(Event)){
			if(Event.Type == sf::Event::Closed)
				window.Close();
				//TODO: remove the gui objects
				//TODO: save file also
			if(Event.Type == sf::Event::MouseButtonPressed)
				Line.SetP1(window.ConvertCoords(Input.GetMouseX(), Input.GetMouseY(), &window.GetView()));
			if(Event.Type == sf::Event::MouseButtonReleased){
				CutBody(body, Line.GetP1(), Line.GetP2());
				Line.SetP2(Line.GetP1()); // to make invisible
			}

			gui.ProcessEvent(&Event);
		}

		world.Step(window.GetFrameTime(), 5, 10);

		if(DoneBtn.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
			sf::Randomizer randGen;
			sf::Body<sf::Shape> physicsBody(body);
			std::vector<my::BodyRep*> bodyReps;
			sf::Clock timer;

			for(int I= 0; I< physicsBody.GetNumbObjects(); I++){ 
				my::BodyRep* bodyRep = new my::BodyRep;
				sf::Vector2f globalPos = physicsBody.TransformToGlobal(physicsBody.GetObject(I)->GetPosition());
				bodyRep->Create(physicsBody.GetObject(I), &world, globalPos, 1);
				bodyRep->Getb2Body()->SetAngularVelocity(randGen.Random(0, 360));
				bodyRep->Getb2Body()->SetLinearVelocity(b2Vec2(randGen.Random(-.2f, .2f), randGen.Random(-.2f, .2f)));
				bodyReps.push_back(bodyRep);
			}

			double stopTime= timer.GetElapsedTime() + 8;
			for(float I= 0; I< stopTime; I= timer.GetElapsedTime()){
				world.Step(window.GetFrameTime(), 5, 10);
				
				// Rendering
				window.Clear(sf::Color(210,210,255));

				for(int i= 0; i< bodyReps.size(); i++){
					bodyReps[i]->Update();
					window.Draw(*bodyReps[i]);
				}

				gui.Draw(window);

				window.Display();
			}

			for(int I= 0; I< bodyReps.size(); I++)
				delete bodyReps[I];

			Done = true;
		}

		if(Input.IsMouseButtonDown(sf::Mouse::Left))
			Line.SetP2(window.ConvertCoords(Input.GetMouseX(), Input.GetMouseY(), &window.GetView()));

		gui.CheckState(&Input);

		//rendering
		window.Clear(sf::Color(210,210,255));

		gui.Draw(window);
		window.Draw(body);
		window.Draw(bodyRep);

		window.Draw(Line);

		window.Display();
	}

	ShowDesignScreen(window, gui, body);

	//restore GUI to original state
	ShowGuiContents(gui, 3, 1);
	Instructions.Show(false);
	DoneBtn.Show(false);
}

bool CutBody(sf::Body<sf::Shape>& body, sf::Vector2f CutP1, sf::Vector2f CutP2){
	int OrigNumbObjs= body.GetNumbObjects();
	int numbCuts= 0; //keeps track of vector length changes
	for(int objI= 0; objI< OrigNumbObjs- numbCuts; objI++){

		const int NONE = -2;
		sf::Shape* shape = body.GetObject(objI);
		sf::Vector2f p1, p2;
		int colIndex1= NONE, colIndex2 = NONE;
		sf::Vector2f colLoc1, colLoc2; 

		// See if any of the bodies shape need to be cut
		for(int i= 0; i< shape->GetNbPoints(); i++){ 
			int test = shape->GetNbPoints();
			p1= body.TransformToGlobal(shape->TransformToGlobal(shape->GetPointPosition(i)));	
			if(i != shape->GetNbPoints() -1)
				p2= body.TransformToGlobal(shape->TransformToGlobal(shape->GetPointPosition(i+ 1)));
			else	
				p2= body.TransformToGlobal(shape->TransformToGlobal(shape->GetPointPosition(0)));

			// Find the two intersect points if there is any
			if(colIndex1 == NONE){
				if(lines_intersect(p1, p2, CutP1, CutP2, colLoc1.x, colLoc1.y) == DO_INTERSECT)
					colIndex1 = i;}     // collision location set in intersect func
			else
				if(lines_intersect(p1, p2, CutP1, CutP2, colLoc2.x, colLoc2.y) == DO_INTERSECT)
					colIndex2 = i;      // collision location set in intersect func
		}

		//Now do actual cutting if there is exactly two points
		if(colIndex1 != NONE && colIndex2 != NONE){

			sf::Shape* half1 = new sf::Shape, *half2 = new sf::Shape;
			half1->SetOutlineWidth(1); half2->SetOutlineWidth(1);

			sf::Color Col1, Col2;
			CreateRandColor(Col1);
			CreateRandColor(Col2);

			//half 1 creation
			half1->AddPoint(body.TransformToLocal(shape->TransformToLocal(colLoc1)), Col1);
			half1->AddPoint(body.TransformToLocal(shape->TransformToLocal(colLoc2)), Col1);
			for(int I= colIndex2 + 1; I != colIndex1 +1; I++){
				if(I == shape->GetNbPoints())
					I = -1; //If shape ends, start from beginning
				else
					half1->AddPoint(shape->GetPointPosition(I), Col1);
			}
			half1->SetPosition(shape->GetPosition());

			//half 2 creation
			half2->AddPoint(body.TransformToLocal(shape->TransformToLocal(colLoc2)), Col2);
			half2->AddPoint(body.TransformToLocal(shape->TransformToLocal(colLoc1)), Col2);
			for(int I= colIndex1 + 1; I != colIndex2 +1; I++){
				if(I == shape->GetNbPoints())
					I = -1; //If shape ends, start from beginning
				else
					half2->AddPoint(shape->GetPointPosition(I), Col2);
			}
			half2->SetPosition(shape->GetPosition());
			//half2->Move(half2->GetPointPosition(0).x, half2->GetPointPosition(0).y);
			//half2->SetCenter(half2->GetPointPosition(0));


			half1->Move(-body.GetPosition());  //body mistakenly changes the 
			half2->Move(-body.GetPosition());  //obj according to it's position
			body.AddObject(*half1);
			body.AddObject(*half2);

			int slope = ( (CutP1.y / CutP2.y) / (CutP1.x / CutP2.x) );{
				if(slope < 0) slope = -slope;

				if(slope < 1){
					half2->Move(0, -5);
					half1->Move(0, 5);
				}
				else if(slope >= 1){
					half2->Move(-5, 0);
					half1->Move(5, 0);
				}
			}

			//account for the newly added shape
			numbCuts++;
			body.DestroyObject(objI);
			objI--;
		}

	} //for each object

	return numbCuts;
}

void CreateRandColor(sf::Color& color){
	sf::Randomizer generator;

	color.r = generator.Random(0, 255);
	color.g = generator.Random(0, 255);
	color.b = generator.Random(0, 255);
}