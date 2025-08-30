#!/bin/bash

CWD=$(pwd)
BUILD_DIR_ABSOLUTE="${CWD}/build"

if [ ! -d "$BUILD_DIR_ABSOLUTE" ]; then
    echo "Error: Build directory '$BUILD_DIR_ABSOLUTE' not found."
    echo "Please compile the project first by running 'cmake' and 'make'."
    exit 1
fi


run_in_parent_dir() {
    EXE_NAME=$1
    shift
    (cd .. && "${BUILD_DIR_ABSOLUTE}/${EXE_NAME}" "$@")
}


case "$1" in
    nonhop-batch)
        echo "Running Non-HOP Maintenance Experiment Suite..."
        run_in_parent_dir label_maintainer musae musae 300 600
        run_in_parent_dir label_maintainer twitch twitch 300 600
        run_in_parent_dir label_maintainer github github 300 600
        run_in_parent_dir label_maintainer reddit reddit 300 600
        run_in_parent_dir label_maintainer dblp dblp 300 600
        run_in_parent_dir label_maintainer dblp amazon 300 600
        run_in_parent_dir label_maintainer liveJournal liveJournal 300 600
        ;;
    hop-batch)
        echo "Running HOP Maintenance Experiment Suite..."
        run_in_parent_dir hop_maintain_exp musae musae 300 600
        run_in_parent_dir hop_maintain_exp twitch twitch 300 600
        run_in_parent_dir hop_maintain_exp github github 300 600
        run_in_parent_dir hop_maintain_exp amazon amazon 300 600
        run_in_parent_dir hop_maintain_exp reddit reddit 300 600
        run_in_parent_dir hop_maintain_exp dblp dblp 300 600
        ;;

    gen-exp)
        echo "Running Label Generation Experiment..."
        run_in_parent_dir label_generator
        ;;
    nonhop-lppr)
        echo "Running Non-HOP Maintenance (Generate LPPR)..."
        run_in_parent_dir label_maintainer
        ;;
    hop-lppr)
        echo "Running HOP Maintenance (Generate LPPR)..."
        run_in_parent_dir hop_maintain_exp
        ;;
    
    gen-test)
        echo "Running Label Generation Test..."
        run_in_parent_dir label_generation_test
        ;;
    nonhop-test)
        echo "Running Non-HOP Maintenance Test..."
        run_in_parent_dir nonhop_maintain_test
        ;;
    hop-test)
        echo "Running HOP Maintenance Test..."
        run_in_parent_dir hop_maintain_test
        ;;
    *)
        echo "Usage: $0 {command}"
        echo ""
        echo "Batch Commands (for full experiments):"
        echo "  nonhop-batch          - Run the full Non-HOP maintenance experiment suite."
        echo "  hop-batch             - Run the full HOP maintenance experiment suite."
        echo ""
        echo "Single Run Commands:"
        echo "  gen-exp               - Run the label generation experiment."
        echo "  nonhop-lppr           - Generate LPPR for Non-HOP maintenance."
        echo "  hop-lppr              - Generate LPPR for HOP maintenance."
        echo ""
        echo "Test Commands:"
        echo "  gen-test, nonhop-test, hop-test"
        exit 0
        ;;
esac

echo "Done."