#!/bin/bash
# scripts/docker_deploy.sh
#
# Docker deployment script for FastQTools
# Supports building and deploying different environments

set -euo pipefail

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Default values
ENVIRONMENT="production"
ACTION="build"
PUSH_REGISTRY=""
TAG="latest"

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --env ENV              Environment to deploy (production|development|test) [default: production]"
    echo "  --action ACTION        Action to perform (build|run|stop|push) [default: build]"
    echo "  --tag TAG              Docker image tag [default: latest]"
    echo "  --registry REGISTRY    Docker registry to push to (for push action)"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Build production image"
    echo "  $0 --env development --action run     # Run development environment"
    echo "  $0 --action push --registry myregistry.com/fastqtools"
    echo "  $0 --env test --action run            # Run tests in container"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --env)
            ENVIRONMENT="$2"
            shift 2
            ;;
        --action)
            ACTION="$2"
            shift 2
            ;;
        --tag)
            TAG="$2"
            shift 2
            ;;
        --registry)
            PUSH_REGISTRY="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Validate environment
case $ENVIRONMENT in
    production|development|test)
        ;;
    *)
        echo -e "${RED}Error: Invalid environment '$ENVIRONMENT'. Must be production, development, or test.${NC}"
        exit 1
        ;;
esac

# Validate action
case $ACTION in
    build|run|stop|push)
        ;;
    *)
        echo -e "${RED}Error: Invalid action '$ACTION'. Must be build, run, stop, or push.${NC}"
        exit 1
        ;;
esac

echo -e "${GREEN}>>> FastQTools Docker Deployment${NC}"
echo -e "${BLUE}Environment: $ENVIRONMENT${NC}"
echo -e "${BLUE}Action: $ACTION${NC}"
echo -e "${BLUE}Tag: $TAG${NC}"

# Change to project root directory
cd "$(dirname "$0")/.."

build_image() {
    local env=$1
    local tag=$2
    
    echo -e "${GREEN}>>> Building $env image...${NC}"
    
    case $env in
        production)
            docker build -f docker/Dockerfile -t "fastqtools:$tag" --target production .
            ;;
        development)
            docker build -f docker/Dockerfile.dev -t "fastqtools-dev:$tag" --target development .
            ;;
        test)
            docker build -f docker/Dockerfile.dev -t "fastqtools-test:$tag" --target development .
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Successfully built $env image${NC}"
    else
        echo -e "${RED}✗ Failed to build $env image${NC}"
        exit 1
    fi
}

run_container() {
    local env=$1
    
    echo -e "${GREEN}>>> Running $env container...${NC}"
    
    case $env in
        production)
            docker-compose -f docker/docker-compose.yml up -d prod
            ;;
        development)
            docker-compose -f docker/docker-compose.yml up -d dev
            ;;
        test)
            docker-compose -f docker/docker-compose.yml up test
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Successfully started $env container${NC}"
        if [ "$env" != "test" ]; then
            echo -e "${BLUE}Container is running. Use 'docker logs fastqtools-$env' to view logs.${NC}"
        fi
    else
        echo -e "${RED}✗ Failed to start $env container${NC}"
        exit 1
    fi
}

stop_container() {
    local env=$1
    
    echo -e "${GREEN}>>> Stopping $env container...${NC}"
    
    case $env in
        production)
            docker-compose -f docker/docker-compose.yml stop prod
            docker-compose -f docker/docker-compose.yml rm -f prod
            ;;
        development)
            docker-compose -f docker/docker-compose.yml stop dev
            docker-compose -f docker/docker-compose.yml rm -f dev
            ;;
        test)
            docker-compose -f docker/docker-compose.yml stop test
            docker-compose -f docker/docker-compose.yml rm -f test
            ;;
    esac
    
    echo -e "${GREEN}✓ Stopped $env container${NC}"
}

push_image() {
    local env=$1
    local tag=$2
    local registry=$3
    
    if [ -z "$registry" ]; then
        echo -e "${RED}Error: Registry not specified for push action${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}>>> Pushing $env image to registry...${NC}"
    
    local image_name
    case $env in
        production)
            image_name="fastqtools:$tag"
            ;;
        development)
            image_name="fastqtools-dev:$tag"
            ;;
        test)
            image_name="fastqtools-test:$tag"
            ;;
    esac
    
    # Tag for registry
    docker tag "$image_name" "$registry:$tag"
    
    # Push to registry
    docker push "$registry:$tag"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Successfully pushed to $registry:$tag${NC}"
    else
        echo -e "${RED}✗ Failed to push to registry${NC}"
        exit 1
    fi
}

# Execute action
case $ACTION in
    build)
        build_image "$ENVIRONMENT" "$TAG"
        ;;
    run)
        build_image "$ENVIRONMENT" "$TAG"
        run_container "$ENVIRONMENT"
        ;;
    stop)
        stop_container "$ENVIRONMENT"
        ;;
    push)
        push_image "$ENVIRONMENT" "$TAG" "$PUSH_REGISTRY"
        ;;
esac

echo -e "${GREEN}>>> Docker deployment completed successfully!${NC}"