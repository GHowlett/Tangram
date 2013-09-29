#ifndef COMMON_PROPERTIES
#define COMMON_PROPERTIES

#include <my/SFML serialization.h>
#include <boost/filesystem.hpp>

const std::string SavesInfo = "SavesInfo";
enum SHAPE{	TRIANGLE= 3, SQUARE= 4, OCTOGAN= 8 }; //TODO: add other shapes
unsigned int PressedBtn= 0;


//////////////////////////////
/// load/save file functions
//////////////////////////////
void LoadBody (sf::Body<sf::Shape>& body, std::string filename){
	if(!boost::filesystem::exists(filename)){
		std::cout << "File does not exists" << std::endl;
		return;}

	std::ifstream ifs(filename);
	boost::archive::text_iarchive ia(ifs);

	ia >> body;

	return;
}

void SaveBody (sf::Body<sf::Shape>& body, std::string filename){
	std::ofstream ofs(filename);
	boost::archive::text_oarchive oa(ofs);

	oa << body;

	return;
}

void LoadSavesInfo(int& numbSaves, std::vector<std::string>& saveNames){
		if(boost::filesystem::exists("SavesInfo")){
			std::ifstream ifs("SavesInfo");
			boost::archive::text_iarchive ia(ifs);

			ia >> numbSaves;
			for(int I= 0; I< numbSaves; I++)
				saveNames.push_back("");
			ia >> saveNames;
		}
	}

void SaveSavesInfo(int& numbSaves, std::vector<std::string>& saveNames){
	std::ofstream ofs("SavesInfo");
	boost::archive::text_oarchive oa(ofs);
		
	oa << numbSaves;
	oa << saveNames;
}

void LoadShape(sf::Body<sf::Shape>& body, int sides){
	sf::Shape* shape = new sf::Shape; //needs to survive for body
	switch(sides){
		case 3:	// Triangle
			shape->AddPoint(sf::Vector2f(0, -500));
			shape->AddPoint(sf::Vector2f(-250, 0));
			shape->AddPoint(sf::Vector2f(250, 0));
			break;
		case 4: ;// Square

		default: std::cout << "Not a supported shape, see the SHAPE:: enum" << std::endl;
	//TODO: create shape based on sides
	} //switch

	shape->SetOutlineWidth(1);
	body.AddObject(*shape);
}

void SetShapeFill(sf::Shape& shape, sf::Color color){
	for(int I= 0; I< shape.GetNbPoints(); I++)
		shape.SetPointColor(I, color);
}

void HideGuiContents(cp::cpGuiContainer& gui, int numbObjs, int numbDrawables){
	for(int I= 0; I< numbObjs; I++)
		gui.control[I]->Show(false);

	for(int I= 0; I< numbDrawables; I++){
		sf::Color newColor(gui.Drawables[I]->GetColor());
		newColor.a = 0.f;
		gui.Drawables[I]->SetColor(newColor);
	}
}

void ShowGuiContents(cp::cpGuiContainer& gui, int numbObjs, int numbDrawables){
	for(int I= 0; I< numbObjs; I++)
		gui.control[I]->Show(true);

	for(int I= 0; I< numbDrawables; I++){
		sf::Color newColor(gui.Drawables[I]->GetColor());
		newColor.a = 255.f;
		gui.Drawables[I]->SetColor(newColor);
	}
}

///////////////////////
///		Line Class	 //
///////////////////////
class Line: public sf::Body<sf::Shape> {
	private:
		sf::Vector2f P1,P2;
		bool Show;
		enum{ RADIUS= 4, WIDTH= 2 };

	public:
		Line(){
			sf::Shape* Line, *Circle1, *Circle2;
			Line = new sf::Shape(sf::Shape::Line(sf::Vector2f(0,0), sf::Vector2f(0,0), WIDTH, sf::Color(50,150,50)));
			AddObject(*Line);
			Circle1 = new sf::Shape(sf::Shape::Circle(sf::Vector2f(0,0), RADIUS, sf::Color(255,0,0)));
			AddObject(*Circle1);
			Circle2 = new sf::Shape(sf::Shape::Circle(sf::Vector2f(0,0), RADIUS, sf::Color(255,0,0)));
			AddObject(*Circle2);

			P1= sf::Vector2f(0,0);
			P2= sf::Vector2f(0,0);
			Show=false;
		}

		void Render(sf::RenderTarget& target) const{
			if(P1 !=  P2)
				sf::Body<sf::Shape>::Render(target);
		}
		//getters and setters
		void SetP1(sf::Vector2f newLoc){ 
			P1= newLoc;
			*GetObject(0) = sf::Shape::Line(P1,P2, WIDTH, sf::Color(50,150,50));
			*GetObject(1) = sf::Shape::Circle(P1, RADIUS, sf::Color(255,0,0));
		}
		void SetP2(sf::Vector2f newLoc){
			P2= newLoc;
			*GetObject(0) = sf::Shape::Line(P1,P2, WIDTH, sf::Color(50,150,50));
			*GetObject(2) = sf::Shape::Circle(P2, RADIUS, sf::Color(255,0,0));
		}

		sf::Vector2f GetP1(){
			return P1 ;}
		sf::Vector2f GetP2(){
			return P2 ;}
}; //Line class


/////////////////////////////////
//serialization of vector<string>
//////////////////////////////////
namespace boost{ namespace serialization{

	template<class Archive>
void save(Archive & ar, const std::vector<std::string>& vec, const unsigned int version)
{
	for(int I= 0; I< vec.size(); I++)
		ar & vec[I];
}

	template<class Archive>
void load(Archive & ar, std::vector<std::string>& vec, const unsigned int version)
{
	for(int I= 0; I< vec.size(); I++){
		std::string tempString;
		ar & tempString;
		vec[I] = tempString;
	}
}

}} //namespaces

BOOST_SERIALIZATION_SPLIT_FREE(std::vector<std::string>);


#endif