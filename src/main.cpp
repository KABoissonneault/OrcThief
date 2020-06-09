#include "Ogre/Root.h"
#include "Ogre/Window.h"
#include "Ogre/WindowEventUtilities.h"
#include "Ogre/Compositor/CompositorManager2.h"

size_t get_number_threads()
{
	return 4; // TODO
}

const Ogre::ColourValue k_background_color(.8f, 0.4f, 0.4f);

class TestWindowEventListener : public Ogre::WindowEventListener
{
	bool m_should_quit = false;

public:
	virtual void windowClosed(Ogre::Window* rw) override { m_should_quit = true; }

	bool should_quit() const noexcept { return m_should_quit; }
};

int main()
{
	Ogre::Root root;
	if (!root.restoreConfig())
	{
		if (!root.showConfigDialog())
			return -1;
	}

	Ogre::Window* const window = root.initialise(true /*create window*/, "Ogre Test");
	
	Ogre::SceneManager* const scene_manager = root.createSceneManager(Ogre::ST_GENERIC, get_number_threads() - 1);

	Ogre::Camera* const camera = scene_manager->createCamera("Main Camera");

	Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
	compositor_manager->createBasicWorkspaceDef("Test Workspace", k_background_color);
	compositor_manager->addWorkspace(scene_manager, window->getTexture(), camera, "Test Workspace", true /*enabled*/);

	TestWindowEventListener listener;
	Ogre::WindowEventUtilities::addWindowEventListener(window, &listener);
	while (true) {
		Ogre::WindowEventUtilities::messagePump();
		if (listener.should_quit()
			|| !root.renderOneFrame()
			) {
			break;
		}
	}

	Ogre::WindowEventUtilities::removeWindowEventListener(window, &listener);
}
