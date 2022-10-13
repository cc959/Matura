cmake_minimum_required(VERSION 3.8)
project( Matura )

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

find_package( OpenCV REQUIRED )
find_package( Corrade REQUIRED Main Utility )
find_package( Magnum REQUIRED GL Text MeshTools Shaders SceneGraph Trade Sdl2Application AnySceneImporter AnyImageImporter Primitives )
find_package( MagnumPlugins REQUIRED GltfImporter StbTrueTypeFont  )
find_package( ImGui REQUIRED )
find_package( MagnumIntegration REQUIRED ImGui )

file(GLOB SourceFiles CONFIGURE_DEPENDS "src/*.cpp")
corrade_add_resource(Images Images.conf)
corrade_add_resource(Shaders Shaders.conf)

include_directories( ${OpenCV_INCLUDE_DIRS} ./ /usr/include/colmap ./include/ /usr/include/MagnumExternal/ImGui/ )

add_executable( ${PROJECT_NAME} ${SourceFiles} ${Images} ${Shaders} )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} ) 
target_link_libraries( ${PROJECT_NAME} PRIVATE Corrade::Main Magnum::Application MagnumPlugins::StbTrueTypeFont Magnum::Text Magnum::GL Magnum::Magnum Magnum::MeshTools Magnum::SceneGraph Magnum::Shaders Magnum::Trade Magnum::AnySceneImporter MagnumPlugins::GltfImporter Magnum::AnyImageImporter MagnumIntegration::ImGui Magnum::Primitives )

add_compile_options( -std=c++20 -g )