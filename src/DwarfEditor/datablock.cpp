#include "datablock.h"

#include "Ogre/Root.h"
#include "Ogre/HlmsManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Unlit/Datablock.h"
#include "Ogre/Components/Hlms/Pbs.h"
#include "Ogre/ArchiveType.h"
#include "Ogre/ArchiveManager.h"
#include "Ogre/HlmsManager.h"


#include <cfloat>

namespace ot::dedit::datablock
{
	void load_hlms(std::filesystem::path const& resource_folder)
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager* const hlms_manager = root.getHlmsManager();

		Ogre::String main_folder;
		Ogre::StringVector library_folders;
		{
			Ogre::HlmsUnlit::getDefaultPaths(main_folder, library_folders);

			auto const main_path = resource_folder / main_folder;
			Ogre::Archive* const main_archive = Ogre::ArchiveManager::getSingleton().load(main_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);

			Ogre::ArchiveVec library_archives(library_folders.size());
			std::transform(library_folders.begin(), library_folders.end(), library_archives.begin(), [&resource_folder](auto const& library_folder)
			{
				auto const library_path = resource_folder / library_folder;
				return Ogre::ArchiveManager::getSingleton().load(library_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);
			});

			// Takes ownership of main_archive, but copies library_archives
			hlms_manager->registerHlms(OGRE_NEW Ogre::HlmsUnlit(main_archive, &library_archives));
		}

		{
			Ogre::HlmsPbs::getDefaultPaths(main_folder, library_folders);

			auto const main_path = resource_folder / main_folder;
			Ogre::Archive* const main_archive = Ogre::ArchiveManager::getSingleton().load(main_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);

			Ogre::ArchiveVec library_archives(library_folders.size());
			std::transform(library_folders.begin(), library_folders.end(), library_archives.begin(), [&resource_folder](auto const& library_folder)
			{
				auto const library_path = resource_folder / library_folder;
				return Ogre::ArchiveManager::getSingleton().load(library_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);
			});

			// Takes ownership of main_archive, but copies library_archives
			auto const pbs_manager = OGRE_NEW Ogre::HlmsPbs(main_archive, &library_archives);
			hlms_manager->registerHlms(pbs_manager);
		}
	}
}
