#include <core\core.h>
namespace cap
{
	RenderWindow*	Core::window;
	Clock			Core::clock;
	Time			Core::time;
	float			Core::deltaTime = 0.0;
	View*			Core::camera;
	Event			Core::event;

	LuaMap			Core::properties;
	Managers		Core::managers;
	TilesetMap		Core::tilesets;
	Levels			Core::levels;
	Forms           Core::forms;
	GUIStack        Core::stack_gui;

    LuaRef          Core::onSetup = 0;
    LuaRef          Core::onClose = 0;
	LuaRef			Core::onUpdate = 0;

	void Core::init(int width, int height, const string& title)
	{
        // Lua init
        Script::init();

        // Data init
		window = new RenderWindow(VideoMode(width, height), title);
		camera = nullptr;

        onSetup = Script::newRef();
        onClose = Script::newRef();
		onUpdate = Script::newRef();

        // GUI init
		ImGui::SFML::Init(*window);

		//////////////////////////////////////////////////////////////////////////////
        GUIForm* test = new GUIForm();
        test->setTitle("Test form");

		GUILabel* label = new GUILabel();
		label->setTitle("Everlasting Summer");
		test->addWidget(label);

        GUIButton* but1 = new GUIButton();
        but1->setTitle("New game");
        test->addWidget(but1);

        GUIButton* but2 = new GUIButton();
        but2->setTitle("Settings");
        test->addWidget(but2);

        GUIButton* but3 = new GUIButton();
        but3->setTitle("Exit");
		but3->onClick = Script::eval("function() print('Ha-Ha-Ha!') end");
        test->addWidget(but3);

        stack_gui.push_back(test);
		///////////////////////////////////////////////////////////////////////////

        // Reset log
        Script::reset_log("Captain Engine log:");

        // Including plugins
		managers["TILED_CSV_LOADER"] = new TiledLoaderCSV();
	}

	void Core::setProperty(const string& name, LuaRef value)
	{
		if (name == "fps")
		{
			window->setFramerateLimit(value);
		}
		else if(name == "window_size")
		{
			window->setSize(value.cast<Point>());
		}
		//properties.set(name, value);
	}

	LuaRef Core::getProperty(const string& name)
	{
		return properties.get(name);
	}

	void Core::initScript(string file)
    {
        // Class init
        initClasses();

        // Run script
        Script::include(CAP_SCRIPTS_DIR + file + ".lua");
		
        // Call onSetup
        if(onSetup.isFunction()) onSetup();
	}

	void Core::eventProcessing()
	{
		while (window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
				if (onClose.isFunction())
				{
					LuaRef result = onClose();
					if (result.isNil() || result == true) window->close();
				}
				else window->close();
            }
		}
	}

	void Core::startLoop()
	{
		while (window->isOpen())
		{
			// Time reset
			time = clock.restart();
			deltaTime = time.asSeconds();

			eventProcessing();
			update();	
			draw();
		}
		ImGui::SFML::Shutdown();
	}

	void Core::update()
	{
		ImGui::SFML::Update(*window, time);
		if (onUpdate.isFunction()) onUpdate();
	}

	void Core::draw()
	{
		window->clear();

        // Draw gui
        for(GUIForm* form : stack_gui) form->draw();
		ImGui::SFML::Render(*window);

		window->display();
	}

	Level* Core::loadLevel(string name, string loader_type)
	{
		/*LevelLoader* loader = lvl_loaders[loader_type];
		if (loader)
		{
			levels[name] = loader->load(CAP_LEVELS_DIR + name);
			return levels[name];
		}
		Script::print_log("Warning! Level loader '" + loader_type + "' is not found, level '" + name + "' is not loaded!");*/
		return nullptr;
	}

	Tileset Core::loadTilesetForLevel(string name, string loader_type)
	{
		/*LevelLoader* loader = lvl_loaders[loader_type];
		if (loader)
		{
			
		}
		Script::print_log("Warning! Level loader '" + loader_type + "' is not found, tilesets is not loaded!");*/
		return nullptr;
	}

    void Core::initClasses()
    {
        Script::global()

			// ------- Class Core ----------------------------------------------- //
			.beginClass<Core>("Core")
			.addStaticFunction("setProperty", &Core::setProperty)
			.addStaticFunction("getProperty", &Core::getProperty)

			.addStaticProperty("onSetup", &Core::onSetup)
			.addStaticProperty("onClose", &Core::onClose)
			.addStaticProperty("onUpdate", &Core::onUpdate)

			.addStaticProperty("deltaTime", &Core::deltaTime, false)

			//.addStaticFunction("loadLevel", &Core::loadLevel)
			.endClass()

			// ------- Class Entity ----------------------------------------------- //
			.beginClass<Entity>("Entity")

			.addFunction("update",		&Entity::update)

			.addFunction("addChild",	&Entity::addChild)

			.addFunction("setName",		&Entity::setName)

			.addFunction("getName",		&Entity::getName)
			.addFunction("getType",		&Entity::getType)
			.addFunction("getParent",	&Entity::getParent)
			.addFunction("getChild",	&Entity::getChild)
			.endClass()

			// ------- Class PointEntity ----------------------------------------------- //
			.deriveClass<PointEntity, Entity>("PointEntity")
			.endClass();
    }
}
