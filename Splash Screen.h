#include <Box2D\Box2D.h>

#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>

#include <my/LineIntersection.h>
#include <my/SFboxML.h> 
#include <my/ShapeFactory.h>

#include <vector>
#include <iostream>

#include <my/Box2D serialization.h>
#include <my/SFML serialization.h>

sf::Mutex globalMutex;
const int numbImgs = 75;

void SubtractColor(sf::Color& col1, sf::Color& col2);
void RecolorImage(sf::Image& image, sf::Color filter, bool subract = false);
sf::Image* RecolorSprite(sf::Sprite& sprite, sf::Color filter, bool subtract = false);
void CreateTransition(void* animationArray);

void ShowSplashScreen(sf::RenderWindow& Window){
	sf::View splashView(sf::FloatRect(0, 0, 800, 600));
	sf::View oldView(Window.GetView());

	Window.SetView(splashView);

	sf::Image Logo_img, Eye_img, TitleScreen_img;
	{
		Logo_img.LoadFromFile("Art\\Logo.png");
		Eye_img.LoadFromFile("Art\\Eye.jpg");
		TitleScreen_img.LoadFromFile("Art\\TitleScreen.jpg");
	}

	Logo_img.CreateMaskFromColor(sf::Color(255, 255, 255));

	Eye_img.CreateMaskFromColor(sf::Color(255, 255, 255, 255));
	RecolorImage(Eye_img, sf::Color(0, 0, 0, 255), true);
	std::vector<sf::Image> fadeAnimation(numbImgs, Eye_img);

	sf::Thread AnimationThread(CreateTransition, reinterpret_cast<void*>(&fadeAnimation[0]));
	AnimationThread.Launch();
	AnimationThread.Wait(); //Takes way too long....

	sf::Sprite Logo(Logo_img, sf::Vector2f(400, 300), sf::Vector2f(0,0));
	sf::Sprite Eye(Eye_img, sf::Vector2f(290, 300), sf::Vector2f(1.35,1.35)); 
	sf::Sprite TitleScreen(TitleScreen_img, sf::Vector2f(400, 300));
	{
		Logo.SetCenter(Logo_img.GetWidth() /2, Logo_img.GetHeight() /2);
		Eye.SetCenter(Eye_img.GetWidth() /2, Eye_img.GetHeight() /2);
		TitleScreen.SetCenter(TitleScreen_img.GetWidth() /2, TitleScreen_img.GetHeight() /2);
		TitleScreen.SetScaleX(((float)Window.GetWidth()) / ((float)TitleScreen_img.GetWidth()));
		TitleScreen.SetScaleY(((float)Window.GetHeight()) / ((float)TitleScreen_img.GetHeight()));
	}
 
	float TimeLapsed= 0;
	int AnimationNumber= 0;
	for(sf::Clock I; I.GetElapsedTime()< 11;){
		if(I.GetElapsedTime() - TimeLapsed >= (1.f/30.f)){
			Window.Clear(sf::Color(255,255,255,255));

			Window.Draw(Eye);
			Window.Draw(Logo);

			TimeLapsed = I.GetElapsedTime();

			if(I.GetElapsedTime() > 5 && I.GetElapsedTime() < 9){
				Eye.SetImage(fadeAnimation[AnimationNumber/2]);
				AnimationNumber++;
			}

			if(I.GetElapsedTime() < 3){ //Logo animation
				Logo.SetScale(Logo.GetScale() + sf::Vector2f(.008, .008)); // Final scale of .9
			}

			if(I.GetElapsedTime() > 9)
				Window.Draw(TitleScreen);

			Window.Display();
		}
	}	
}

void CreateTransition(void* animationArray){
	sf::Image* aray = reinterpret_cast<sf::Image*> (animationArray);

	void threadFunc(void* imgArray);
	sf::Thread thread(threadFunc, reinterpret_cast<void*>(aray));

	thread.Launch();
	thread.Wait();
}

void threadFunc(void* ptr){
	sf::Image* aray = reinterpret_cast<sf::Image*> (ptr); 

	int AlphaVal = 3.4;

	for(int I= 1; I< numbImgs; I++){
		RecolorImage(aray[I], sf::Color(0, 0, 0, AlphaVal));
		AlphaVal += 3.4;
	}
}

// @Return Ptr: a pointer to the stack allocated image so the 
//				user can deallocate it later
sf::Image* RecolorSprite(sf::Sprite& sprite, sf::Color filter, bool subtract){
	// the image has to survive so it's put ont he stack
	sf::Image* image = new sf::Image;
	*image = *sprite.GetImage();

	RecolorImage(*image, filter, subtract);
	sprite.SetImage(*image);

	return image;
}

void RecolorImage(sf::Image& image, sf::Color filter, bool subtract){
	for( int x= 0; x< image.GetWidth(); x++){
		for(int y= 0; y< image.GetHeight(); y++){
			if(subtract){
				sf::Color pixel = image.GetPixel(x, y);
				SubtractColor(pixel, filter);
				image.SetPixel(x, y, pixel);
			}
			else
				image.SetPixel(x, y, image.GetPixel(x, y) + filter);
		}
	}
}

// int used to stop illegal operations on unsigned chars 
void SubtractColor(sf::Color& col1, sf::Color& col2){
	int diff = ((int)col1.r) - ((int)col2.r);
		if(diff >= 0)
			col1.r -= col2.r;
		else
			col1.r = 0;

	diff = ((int)col1.g) - ((int)col2.g);
		if(diff >= 0)
			col1.g -= col2.g;
		else
			col1.g = 0;

	diff = ((int)col1.b) - ((int)col2.b);
		if(diff >= 0)
			col1.b -= col2.b;
		else
			col1.b = 0;

	diff = ((int)col1.a) - ((int)col2.a);
		if(diff >= 0)
			col1.a -= col2.a;
		else
			col1.a = 0;
}