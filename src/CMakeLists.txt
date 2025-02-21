target_sources(slippy-map
    PUBLIC
    tiles/TileDownloader.hpp
    tiles/Utils.hpp
    tiles/Tile.hpp
    main.cpp

    PRIVATE
    tiles/TileDownloader.cpp
    tiles/Utils.cpp
)