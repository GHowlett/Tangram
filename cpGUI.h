

// This is a function that will later be attached to a button;
// to be attached a function's parameters must be (Input*, cpObject*)
void ButtonFunction(const sf::Input* Input, cp::cpObject* Object);
int ButFuncInt; // a variable to be affected by the function


int main()
{
	// The original window coordinates are what form the coordinate system for
	// the objects in the GUI
	sf::RenderWindow App(sf::VideoMode(800,650,32), "cpGUI");
	App.SetPosition(0,0);

	// Create a GuiContainer to contain all of the controls we create.
	// This is used mainly for setting focus.
	// YOU HAVE TO HAVE ONE OF THESE or nothing will work.
	cp::cpGuiContainer myGUI(App);

	// *** Create a non-editable read only Text Box & add some text ***
	// Parameters: 1. the parent render window; 2. your GuiContainer;
	// 3&4. the x & y coordinates; 5&6. the width & height
	cp::cpTextBox textBox(&App, &myGUI, 20,20,400,300);

	// When setting the font size, the current font is reloaded with the new
	// size for better quality.
	textBox.SetFontSize(14);
	std::vector<std::string> Texts;
	Texts.push_back("- Here Is an example of text wrapping abilities (this is one line if you look at the source code)");
	Texts.push_back("- Here's an example of a \nNewline");
	Texts.push_back("- Here's another line of text");
	Texts.push_back("\n\n");
	Texts.push_back("_ New features added by **Griffin | Howlett** _");
	Texts.push_back("");
	Texts.push_back("* Fixed everything that was broken");
	Texts.push_back("* Some improvements to abilities and feel of the GUI");
	Texts.push_back("     * Such as text input boxes now showing light \n         text instructions until clicked");
	Texts.push_back("       Or buttons not acivating unless the \n         mouse is still over it when released");
	Texts.push_back("* option to attach a function to any cpObject \n  (called when the mouse is releaed)");
	Texts.push_back("* option of whether to make the GUI stay \n  on screen no matter what view is set (Static)");
	Texts.push_back("* ability to add any sf::Drawable to the GUI");
	Texts.push_back("");
	Texts.push_back("All the commented out code in the source is \n  stuff that has become obsolete due to my edits");
	Texts.push_back("");
	Texts.push_back("Many thanks to Jason Cupp for the original code \n    (even if there was WAY to many WTF moments)");
	Texts.push_back("\nHope this helps SOMEONE out there........");
	textBox.SetText(Texts);

	//Or you can simply load a text file to insert into the texbox
	//textBox.LoadFile("license.txt");


	// *** Create a Selection Box and populate it with choices ***

	cp::cpSelectionBox selBox(&App, &myGUI, 450, 70, 200, 150);
	std::string choices[] = {"Dwarf", "Elf", "Warrior", "Wizard",
		"Adventurer", "Guild Leader", "Cleric", "eight", "nine",
		"ten", "eleven", "twelve", "thirteen", "fourteen"};
	selBox.AddChoices(choices, 14);
	selBox.SetFontSize(20);

	// *** Create a Drop Down Box and populate it with choices ***

	cp::cpDropDownBox dropdownBox(&App, &myGUI, "Select your weapon", 450, 250, 200, 20);
	// MaxDepth is the maximum height the overall DropDown can be (in pixels)
	dropdownBox.SetMaxDepth(300);
	std::string choices2[] = {"Broad sword", "Axe", "Crossbow", "Long bow", "Mace"};
	dropdownBox.AddChoices(choices2, 5);

	// *** Create an Image button using an sf::Image ***
	// These buttons do not have a label, and the size is based on the image's size

	sf::Image image;
	image.LoadFromFile("dragon.png");
	cp::cpImageButton imageBtn(&App, &myGUI, &image, 50, 400);


	// *** Create a regular Button with a label ***
	// if you don't set the button big enough for the text,
	// the button will automatically be resized.
	// default fontsize is 20, you can resize the font and the button
	// later.

	cp::cpButton btn(&App, &myGUI, "Exit Game", 200, 440, 70, 30);
	btn.SetFunction(&ButtonFunction);


	// *** Create a Shape Button from an sf::Shape ***
	//TODO: calculate whether mouse is inside with custom equation that works with any shape
	//      right now it only works with rectangle shapes
	sf::Shape shape;
	shape.AddPoint(200,200, sf::Color::Red);
	shape.AddPoint(250,300, sf::Color::Yellow);
	shape.AddPoint(150,300, sf::Color::Blue);
	cp::cpShapeButton shapeBtn(&App, &myGUI, &shape, 350, 420);


	// *** Create a simple on/off Check Box ***
	// The size of the box is based on the size of the Label's font

	cp::cpCheckBox checkBox(&App, &myGUI, "Play background music", 500, 480);
	checkBox.SetFontSize(20);
	checkBox.SetChecked(true); // unchecked by default
	checkBox.SetMouseoverColor(sf::Color(200,100,100));

	// *** Create a Text Input Box which is a one line input box ***
	// We will also create an sf::String do display above it for a description

	sf::String textInputString("Enter your name:", sf::Font::GetDefaultFont(), 20);
	textInputString.SetPosition(500, 400);
	textInputString.SetColor(sf::Color::Black);

	myGUI.AddObject(textInputString); // the first parameter is the slightly transparent that will be displayed until the box is clicked
	cp::cpTextInputBox textInputBox(&App, &myGUI, "type here", 500, 430, 230, 25);

	//A view will not effect the GUI item unless it's IsStatic bool member is set to false
	sf::Vector2f Center(0,0);
	sf::Vector2f HalfExtents(400,300);
	sf::View View1(Center, HalfExtents);
	View1.Zoom(3);
	App.SetView(View1);
	cp::cpButton ViewEffectedButton(&App, &myGUI, "View Effected Button", -90, 65, 20, 50);
	ViewEffectedButton.IsStatic = false; // Like this

	const sf::Input& input = App.GetInput();

	while(App.IsOpened())
	{

		// The standard Event loop

		sf::Event Event;
		while(App.GetEvent(Event))
		{
			if(Event.Type == sf::Event::Closed)
				App.Close();
			if((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
				App.Close();

			//textInputBox.ProcessTextInput(&Event);
			//textInputBox2.ProcessTextInput(&Event);

			// This is the function that takes care of which control
			// has focus out of all the controls registered to your
			// GuiContainer. It also takes care of any textInputBoxes 
			// that depend on on the key data
			myGUI.ProcessEvent(&Event);
		}

		// we can opptionally check the state of every control 
		// we create using your Input as a parameter.
		// Most controls will return an integer that will tell you the
		// mouse activity within that control. Get the enumerations vallues in cpGUI_Base.h
		// you can use these values to decide what to do.

		//eg:

		int selection; 
		selection = selBox.GetSelection();
		if(selection == 10)
			selBox.RemoveLastChoice();

		std::string name;
			name = textInputBox.GetText();

		/// You must call this every frame as so the 
		/// GUI contents can be updated
		myGUI.CheckState(&input);

		App.Clear(sf::Color(180,180,180));

		//dropdownBox.Draw(); // drop down boxes should always be drawn last
		//textBox.Draw();
		//selBox.Draw();
		//imageBtn.Draw();
		//btn.Draw();
		//shapeBtn.Draw();
		//checkBox.Draw();
		//App.Draw(textInputString);
		//textInputBox.Draw();

		// This draws all the GUIContainers contents
		// This should be called every frame
		// myGUI.Draw(App);

		App.Display();
	}
	return EXIT_SUCCESS;
}

// TODO: check if you can statically convert a function ptr 
void ButtonFunction(const sf::Input* Input, cp::cpObject* Object){
	ButFuncInt= 10;
};
