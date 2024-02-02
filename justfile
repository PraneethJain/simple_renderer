br variant: build (run-all variant)

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all variant: (run-point variant) (run-directional variant) (run-many variant) (run-donut variant)

run-point variant:
    src/build/render "assignment_2/Question 2/CornellBox/point_light.json" outputs/point_light_{{variant}}.png {{variant}}
    src/build/render "assignment_2/Question 1/CornellBox/point_light.json" outputs/point_light_no_texture.png {{variant}}

run-directional variant:
    src/build/render "assignment_2/Question 2/CornellBox/directional_light.json" outputs/directional_light_{{variant}}.png {{variant}}
    src/build/render "assignment_2/Question 1/CornellBox/directional_light.json" outputs/directional_light_no_texture.png {{variant}}

run-many variant:
    src/build/render "assignment_2/Question 2/CornellBox/many_lights.json" outputs/many_lights_{{variant}}.png {{variant}}
    src/build/render "assignment_2/Question 1/CornellBox/many_lights.json" outputs/many_lights_no_texture.png {{variant}}

run-donut variant:
    src/build/render "assignment_2/Question 2/Donuts/scene.json" outputs/donuts_{{variant}}.png {{variant}}
    src/build/render "assignment_2/Question 1/Donuts/scene.json" outputs/donuts_no_texture.png {{variant}}
