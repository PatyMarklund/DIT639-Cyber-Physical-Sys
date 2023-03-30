# Group 14 project

## Description
To be updated when we receive more information about the course.

## How to run our project

This guide is written to work on Ubuntu Desktop LTS 22.04

1. Open a terminal.

2. Before starting make sure you install git and docker if you do not have it already.
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git docker
```
3. Navigate to where you want to store the project files and create a new directory and open it.

```
mkdir 2023-group-14
cd  2023-group-14
```

4. Clone the git repository into the folder.
```
git clone https://git.chalmers.se/courses/dit638/students/2023-group-14.git
```

5. Open the directory.
```
cd UPDATE ME
```

6. Build the docker image.
```
docker build -t group-14/release:latest -f dockerfile .
```

7. Run the docker image. The image needs a decimal number as argument to work.
```
docker run group-14/release:latest (ARGUMENT)
```

## Roadmap
2023-03-31 Basic Git Repository Structure Finished

To be updated...

## How are we working?
For software development, we will actively work with feature branches.
We have the protected main branch where no developer cannot push any work directly to.  To develop a new feature, a developer shall create a new branch with a meaningful branch name. All development effort shall be made in the feature branch including potential unit tests. For larger features, work can take place in sub-branches to make code reviews more manageable and allow for several developers to work on the same feature in parallel. When the feature branch works properly with the current main branch, the developer can make a merge request and this request can be either accepted or rejected depending on the code review result by peer developers.

## Code review
Pull requests shall be reviewed by at least one peer developer. The reviewer shall ensure that the submitted code is in accordance with the project's guidelines and works as described. If the request is approved, it can be merged by the reviewer directly, if it is not approved, the reason shall be made clear using comments. These comments shall be kind and written with a focus on the code, not on the author.

## Commit guidelines
Commit messages shall follow best practices. 
- All commits shall be made with a meaningful commit message; this shall focus on why these changes were made, not how they were made
- If needed, commit messages shall include an appropriate keyword that are supported by GitLab ([More about keywords in Gitlab](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#default-closing-pattern) ).
- Each commit shall include a relevant issue number.

Example of proper commit messages:
```
Add a unit test to ensure prime number check functionality, relates to #12
```
```
Add a unit test, related to issue #4
```
```
Implement #20 to reduce the docker image size
```

## Used tools
- [Docker](https://www.docker.com/)
- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- [Make](https://www.gnu.org/software/make/manual/make.html) 
- G++
- [Ubuntu](https://ubuntu.com/)
- [Runner](https://docs.gitlab.com/runner/)
- [Catch2](https://github.com/catchorg/Catch2) *If we decide to do testing*

## [MIT License](https://opensource.org/license/mit/)

Copyright (c) [2023] [2023-group-14]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


