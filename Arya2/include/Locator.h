#pragma once

namespace Arya
{
    class FileSystem;
    class ModelManager;
    class MaterialManager;
    class TextureManager;
    class Audio;

    class Locator
    {
        public:
            static FileSystem& getFileSystem() { return *fileSystem; }
            static ModelManager& getModelManager() { return *modelManager; }
            static MaterialManager& getMaterialManager() { return *materialManager; }
            static TextureManager& getTextureManager() { return *textureManager; }
            static Audio& getAudio() { return *audio; }

            static void provide(FileSystem* f) { fileSystem = f; }
            static void provide(ModelManager* m) { modelManager = m; }
            static void provide(MaterialManager* m) { materialManager = m; }
            static void provide(TextureManager* t) { textureManager = t; }
            static void provide(Audio* a) { audio = a; }
        private:
            static FileSystem* fileSystem;
            static ModelManager* modelManager;
            static MaterialManager* materialManager;
            static TextureManager* textureManager;
            static Audio* audio;
    };
}
