br spp variant: build (run-all spp variant)

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all spp variant: (run-q1 spp variant) (run-q2 spp variant) (run-q3 spp variant)

run-q1 spp variant:
    src/build/render "assignment_3/Question 1/directional_light.json" "outputs/q1.png" {{spp}} {{variant}}

run-q2 spp variant:
    src/build/render "assignment_3/Question 2/scene1.json" "outputs/q2_1.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 2/scene2.json" "outputs/q2_2.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 2/scene3.json" "outputs/q2_3.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 2/scene4.json" "outputs/q2_4.png" {{spp}} {{variant}}

run-q3 spp variant:
    src/build/render "assignment_3/Question 3/big.json" "outputs/{{variant}}/q3_big_{{spp}}.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 3/many.json" "outputs/{{variant}}/q3_many_{{spp}}.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 3/med.json" "outputs/{{variant}}/q3_med_{{spp}}.png" {{spp}} {{variant}}
    src/build/render "assignment_3/Question 3/small.json" "outputs/{{variant}}/q3_small_{{spp}}.png" {{spp}} {{variant}}