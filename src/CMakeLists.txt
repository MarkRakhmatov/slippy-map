target_sources(slippy-map
    PUBLIC
    tiles/TileDownloader.hpp
    tiles/Utils.hpp
    tiles/Tile.hpp
    tiles/Renderer.hpp
    tiles/MapModel.hpp
    main.cpp

    PRIVATE
    tiles/TileDownloader.cpp
    tiles/Utils.cpp
    tiles/Renderer.cpp
    tiles/MapModel.cpp
)