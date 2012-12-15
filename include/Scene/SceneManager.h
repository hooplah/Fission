#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <vector>

#include "Core/Manager.h"

#include "Scene.h"

class Component;

typedef Component *(*ComponentCreationFunction)(GameObject *);

class SceneManager : public Manager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual bool update(float dt);

        //void saveScene(std::string fileName){mCurrentScene->save(fileName);}
        //void loadScene(std::string fileName){mCurrentScene->load(fileName);}

        GameObject *createGameObject();
        void addGameObject(GameObject *object){mCurrentScene->addGameObject(object);}
        void destroyGameObject(GameObject *object){mCurrentScene->destroyGameObject(object);}

        void clearScene(){mCurrentScene->clear();}

        void registerComponentCreationFunction(std::string name, ComponentCreationFunction funcPointer);
        ComponentCreationFunction getComponentCreationFunction(std::string name);
        void removeComponentCreationFunction(std::string name);

        //accessors
        Scene *getCurrentScene(){return mCurrentScene;}

        static SceneManager *get(){return Instance;}

    protected:
        std::vector <Scene*> mScenes;
        Scene *mCurrentScene;

        std::vector <ComponentCreationFunction> mComponentCreationFunctions;
        std::vector <std::string> mComponentCreationFunctionNames;

    private:
        static SceneManager *Instance;
};

#endif // SCENEMANAGER_H