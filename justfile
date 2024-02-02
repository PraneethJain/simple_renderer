default:
  just --list

alias a1 := assignment-1
alias a2 := assignment-2


prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

assignment-1 variant: build (run-all-1 variant)

assignment-2 variant: build (run-all-2 variant)

build:
    make -j8 -C src/build

run-all-2 variant: (run-point variant) (run-directional variant) (run-many variant) (run-donut variant)

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

run-all-1 variant: (run-cornell variant) (run-donuts variant) (run-tabletop variant)

run-cornell variant: (run-cornell-low variant) (run-cornell-high variant)

run-cornell-low variant:
    src/build/render "assignment_1/Question 2/CornellBox/scene_lo_poly.json" outputs/cornell_low.png {{variant}}

run-cornell-high variant:
    src/build/render "assignment_1/Question 2/CornellBox/scene_hi_poly.json" outputs/cornell_high.png {{variant}}

run-donuts variant:
    src/build/render "assignment_1/Question 2/Donuts/scene.json" outputs/donuts.png {{variant}}

run-tabletop variant:
    src/build/render "assignment_1/Question 2/TableTop/scene.json" outputs/tabletop.png {{variant}}

