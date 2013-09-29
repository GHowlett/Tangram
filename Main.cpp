//included here so it's before gl.h, 
//for later use in Body.h
//#include <SFML/Graphics/GLEW/glew.h>

#include <Box2D\Box2D.h>

#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>

#include <my/LineIntersection.h>
//#include <my/SFboxML.h> 
//#include <my/ShapeFactory.h>

#include <vector>
#include <iostream>

#include <my/Box2D serialization.h>
#include <my/SFML serialization.h>
#include <my/mySerialization.h>
#include <cpGUI/cpGUI.h>

#include "Design Screen.h"
#include "Splash Screen.h"
#include "Cut Screen.h"
#include "Common Properties.h"

int main()
{
	sf::RenderWindow Window(sf::VideoMode(800, 600), "Griffin Howlett", sf::Style::Resize | sf::Style::Close );

	//ShowSplashScreen(Window); //TODO: put this in a thread

	cp::cpGuiContainer GUI(Window);
		const unsigned int XAlign = 550;
		const unsigned int YSpace = 60;
		const unsigned int WidthAlign = 75;
		const unsigned int HeightAlign = 75;

		sf::Image TitleScreen_img;
			TitleScreen_img.LoadFromFile("Art\\TitleScreen.jpg");

			sf::Sprite TitleScreen(TitleScreen_img, sf::Vector2f(400, 300));
			TitleScreen.SetCenter(TitleScreen_img.GetWidth() /2, TitleScreen_img.GetHeight() /2);
			TitleScreen.SetScaleX(((float)Window.GetWidth()) / ((float)TitleScreen_img.GetWidth()));
			TitleScreen.SetScaleY(((float)Window.GetHeight()) / ((float)TitleScreen_img.GetHeight()));

		GUI.AddObject(TitleScreen);

		cp::cpButton NewBtn(&Window, &GUI, "New          ", XAlign, 270, 80, 80);
			NewBtn.SetBackgroundColor(sf::Color(160,82,45,210));
			NewBtn.SetLabelColor(sf::Color(245,232,190));
			NewBtn.SetFontSize(35);
		cp::cpButton LoadBtn(&Window, &GUI, "Load         ", XAlign, 270 + YSpace, 80, 80);
			LoadBtn.SetBackgroundColor(sf::Color(160,82,45,210));
			LoadBtn.SetLabelColor(sf::Color(245,232,190));
			LoadBtn.SetFontSize(35);
		cp::cpButton NewLoadBtn(&Window, &GUI, "New from Save", XAlign, 270 + (2*YSpace), 80, 80);
			NewLoadBtn.SetBackgroundColor(sf::Color(160,82,45,210));
			NewLoadBtn.SetLabelColor(sf::Color(245,232,190));
			NewLoadBtn.SetFontSize(30);
		cp::cpButton OkBtn(&Window, &GUI, "      OK      ", XAlign, 35, 180, 80);
			OkBtn.SetBackgroundColor(sf::Color(160,255,160,210));
			OkBtn.SetFontSize(30);
			OkBtn.Show(false);

		cp::cpTextInputBox SaveInput(&Window, &GUI, "Type file name", XAlign-200, 35, 150, 20);
		SaveInput.Show(false);
		
		int numbSaves =0;
		std::vector<std::string> SaveNames;
		LoadSavesInfo(numbSaves, SaveNames);

		cp::cpDropDownBox SaveDropDown(&Window, &GUI, "Select File", XAlign, 270 + (3*YSpace), 230, 30);
			SaveDropDown.SetMaxDepth(400);
			SaveDropDown.Show(false);
		
			if(numbSaves > 0)
				SaveDropDown.AddChoices(&SaveNames[0], SaveNames.size());
			else
				SaveDropDown.SetLabelText("No Saves");

		cp::cpDropDownBox ShapeDropDown(&Window, &GUI, "Choose Shape", XAlign, 270 + (3*YSpace), 230, 30);
		ShapeDropDown.SetMaxDepth(400);
		ShapeDropDown.Show(false);
		std::string shapeChoices[4] = {"Triangle", "Square", "Pentagon"};
		ShapeDropDown.AddChoices(shapeChoices, 3);


	const sf::Input& Input = Window.GetInput();
	

	//TODO: put wait function here for thread;
	while(Window.IsOpened()){

		sf::Event Event;
		while(Window.GetEvent(Event))
		{
			if(Event.Type == sf::Event::Closed)
				Window.Close();
				//TODO: save file also

			GUI.ProcessEvent(&Event);
		}

		const unsigned int LOAD= 1, NEW= 2, NEWLOAD= 3;
		if(LoadBtn.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
			SaveDropDown.Show(true);
			PressedBtn= LOAD;
			ShapeDropDown.Show(false);}

		if(NewBtn.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
			ShapeDropDown.Show(true);
			PressedBtn= NEW;
			SaveDropDown.Show(false);}

		if(NewLoadBtn.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
			SaveDropDown.Show(true);
			PressedBtn= NEWLOAD;
			ShapeDropDown.Show(false);}

		if(ShapeDropDown.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED || 
		   SaveDropDown.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED  ||
		   SaveDropDown.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
				SaveInput.Show(true);
				OkBtn.Show(true);
		}

		if(OkBtn.CheckState(&Input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED){
			if(PressedBtn == NEW && ShapeDropDown.GetSelection() >= 0 && SaveInput.GetText() != "Type file name"){
				sf::Body<sf::Shape> shape;
				LoadShape(shape, ShapeDropDown.GetSelection()+3);
				ShowCutScreen(Window, GUI, shape);

				//TODO: put the new save's name in SavesNames
				numbSaves++;
				SaveNames.push_back(SaveInput.GetText());
				SaveSavesInfo(numbSaves, SaveNames);
				SaveBody(shape, SaveInput.GetText());
			}
			if(PressedBtn == LOAD && SaveDropDown.GetSelection() >= 0){
				sf::Body<sf::Shape> save;
				LoadBody(save, SaveNames[SaveDropDown.GetSelection()] );
				ShowDesignScreen(Window, GUI, save);

				//update the body on hardrive
				SaveBody(save, SaveNames[SaveDropDown.GetSelection()] );
			}
			if(PressedBtn == NEWLOAD && SaveDropDown.GetSelection() >= 0 && SaveInput.GetText() != "Type file name"){
				sf::Body<sf::Shape> save;
				LoadBody(save, SaveNames[SaveDropDown.GetSelection()]);
				ShowCutScreen(Window, GUI, save);

				numbSaves++;
				SaveNames.push_back(SaveInput.GetText());
				SaveSavesInfo(numbSaves, SaveNames);
				SaveBody(save, SaveInput.GetText());
			}
		}

		GUI.CheckState(&Input);

		Window.Clear(sf::Color(180,180,180));
		GUI.Draw(Window);
		Window.Display();
	}
}

