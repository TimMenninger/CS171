#include "interpolate.h"

using namespace std;
using namespace Eigen;

// The index in keyframeFiles that our p_i is at.
int kfIdx;
// Directory where we store interpolated frames
string outDir;
// Base name of files
string baseName;
// Faces in all objects
vector<tuple<int, int, int>> faces;
// Vector of keyframe files.  We include the first frame twice and the last
// frame three times so we can do Catmull-Rom properly on every keyframe
vector<string*> keyframeFiles;
// Only keep track of the four keyframes we need and buffer the next one
vector<Vertex*> *keyframes[KF_BUF_SIZE];
// List of indices corresponding to keyframes
vector<int> keyframeIdx;

// Function declarations
void init();
void cleanup();
void interpolate();
void computeFrame(MatrixXd uB, vector<Vertex*> *v0, vector<Vertex*> *v1,
    vector<Vertex*> *v2, vector<Vertex*> *v3, vector<Vertex*> **vOut);
void clearVertices(vector<Vertex*> *vertices);
void outputObj(int frameNum, vector<Vertex*> *obj, bool *running);
void readObjFile(string *fn, vector<Vertex*> *vertices, bool getFaces);
void initKeyframeBuffers();

// Function definitions
void init() {
    // First line should be the output directory
    if (!cin.eof())
        cin >> outDir;
    if (!cin.eof())
        cin >> baseName;

    // The rest of the lines are keyframe files. Because the interpolation
    // requires two keyframes before and after, we need to include the first
    // frame twice and the last frame 3 times.
    keyframeFiles.push_back(NULL); // Will copy first frame into this slot
    keyframeIdx.push_back(0); // Will copy first frame into this slot
    string *buffer;
    int frameIdx;
    while (!cin.eof()) {
        buffer = new string;
        cin >> *buffer;
        if (buffer->length() == 0) {
            delete buffer;
            continue;
        }
        keyframeFiles.push_back(buffer);
        cin >> frameIdx;
        keyframeIdx.push_back(frameIdx);
    }

    // If we have no files, there was an error
    if (!keyframeFiles.size()) {
        cout << "error: no keyframe OBJ files given" << endl;
        exit(1);
    }

    // Populate the first item in the vector with a copy of the first keyframe
    keyframeFiles[0] = keyframeFiles[1];
    keyframeIdx[0] = keyframeIdx[1];

    // Put two more copies of the last keyframe
    keyframeFiles.push_back(keyframeFiles[keyframeFiles.size()-1]);
    keyframeFiles.push_back(keyframeFiles[keyframeFiles.size()-1]);
    keyframeIdx.push_back(keyframeIdx[keyframeIdx.size()-1]);
    keyframeIdx.push_back(keyframeIdx[keyframeIdx.size()-1]);

    // Initialize keyframes to all NULL pointers so we can check before delete
    for (int i = 0; i < KF_BUF_SIZE; ++i)
        keyframes[i] = NULL;

    // First keyframe index is at index 1 (which uses keyframe before it)
    kfIdx = 1;

    // Use the files now to fill the keyframe vertex buffers
    initKeyframeBuffers();
}

void cleanup() {
    // Delete vertex vectors
    for (int i = 0; i < KF_BUF_SIZE; ++i) {
        if (keyframes[i] and i > 0 and keyframes[i] != keyframes[i-1]) {
            clearVertices(keyframes[i]);
            delete keyframes[i];
        }
    }

    // Delete keyframe filenames.  Recall that we copy the first one once
    // and the last one twice.
    for (int i = 1; i < keyframeFiles.size()-2; ++i)
        delete keyframeFiles.at(i);
}

void clearVertices(vector<Vertex*> *vertices) {
    // Must delete every Vertex pointer
    for (int i = 0; i < vertices->size(); ++i) {
        delete vertices->at(i);
    }
    vertices->clear();
}

void initKeyframeBuffers() {
    // Spawn up to 4 threads, one for each file we are using to fill a buffer.
    // We need exactly four for this algorithm, so I am fine hard-coding it.
    assert(KF_BUF_SIZE >= 4 and keyframeFiles.size() >= 4);
    vector<thread> threads;
    for (int i = 3; i >= 0; --i) {
        // Don't read same file twice
        if (i < 3 and keyframeFiles.at(i) == keyframeFiles.at(i+1)) {
            keyframes[i] = keyframes[i+1];
            continue;
        }

        // Spawn thread to read file
        keyframes[i] = new vector<Vertex*>;
        threads.push_back(
            thread(readObjFile, keyframeFiles[i], keyframes[i], (i == 0))
        );
    }

    // Join all of the threads before returning, so we know we loaded all files
    for (int i = 0; i < threads.size(); ++i)
        threads[i].join();
}

void loadNextFile(thread **t) {
    assert (t);
    // The buffer is cicrular, so get the appropriate indices
    int nextIdx = (kfIdx+3) % KF_BUF_SIZE;
    if (kfIdx+3 < keyframeFiles.size()) {
        if (!keyframes[nextIdx] or keyframes[nextIdx] == keyframes[(nextIdx+1) % KF_BUF_SIZE])
            keyframes[nextIdx] = new vector<Vertex*>;
        if (keyframeFiles[kfIdx+3] != keyframeFiles[kfIdx+2]) {
            clearVertices(keyframes[nextIdx]);
            *t = new thread(readObjFile,
                keyframeFiles[kfIdx+3], keyframes[nextIdx], false);
        } else
            keyframes[nextIdx] = keyframes[(nextIdx-1) % KF_BUF_SIZE];
    }
}

void computeFrame(MatrixXd uB, vector<Vertex*> *vec0, vector<Vertex*> *vec1,
    vector<Vertex*> *vec2, vector<Vertex*> *vec3, vector<Vertex*> *vecOut) {

    // Treat vertex structs as arrays of doubles
    double *v0, *v1, *v2, *v3, *vOut;

    // Dummy vertex in output
    vecOut->push_back(NULL);
    // Compute a new vertex for every one in the arguments
    for (int i = 1; i < vec0->size(); ++i) {
        vecOut->push_back(new Vertex(0, 0, 0));
        v0 = (double *) vec0->at(i);
        v1 = (double *) vec1->at(i);
        v2 = (double *) vec2->at(i);
        v3 = (double *) vec3->at(i);
        vOut = (double *) vecOut->at(i);

        // Compute point from these in the respective dimensions
        for (int j = 0; j < 3; ++j) {
            MatrixXd p(4, 1);
            p << v0[j], v1[j], v2[j], v3[j];
            vOut[j] = (uB * p)(0);
        }
    }
}

void outputObj(int frameNum, vector<Vertex*> *obj, bool *running) {
    // Generate filename
    ostringstream os;
    os << setfill('0') << setw(NUM_DIGITS) << frameNum;
    string filename = outDir + baseName + os.str() + string(".obj");

    // Open the file for writing
    ofstream outObj(filename);
    if (!outObj.is_open()) {
        cout << "error outputting obj" << endl;
        exit(1);
    }

    // Write all vertices
    for (int i = 1; i < obj->size(); ++i) {
        Vertex v = *(obj->at(i));
        outObj << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    // Write all faces
    for (int i = 0; i < faces.size(); ++i) {
        outObj << "f " << get<0>(faces[i]) << " " << get<1>(faces[i])
               << " " << get<2>(faces[i]) << "\n";
    }

    // Close the file
    outObj.close();

    if (running != NULL)
        *running = false;
}

void interpolate() {
    // Frame we are interpolating
    int frame = keyframeIdx[0];
    vector<thread*> threads;
    threads.push_back(NULL);
    // Allows us to ensure max number of threads without blocking to join
    bool running[MAX_THREADS-1];

    while (kfIdx+2 < keyframeIdx.size()) {
        // Load next file
        threads[0] = NULL;
        loadNextFile(&threads[0]);

        // Interpolate frames and write their outputs on different threads
        float step = 1 / (float) (keyframeIdx[kfIdx+1] - keyframeIdx[kfIdx]);
        for (float uFloat = 0; frame != keyframeIdx[kfIdx+1]; uFloat += step) {
            // Compute u matrix and multiply it by inverse constraint matrix
            MatrixXd u(1, 4);
            u << 1, uFloat, uFloat*uFloat, uFloat*uFloat*uFloat;
            fillConstraintMatrix();
            MatrixXd uB = u*B;

            // Create the set of vertices for this frame
            vector<Vertex*> *vOut = new vector<Vertex*>;

            // Using 0 for time i-1 to 3 for time i+2
            vector<Vertex*> *v0 = keyframes[(kfIdx-1) % KF_BUF_SIZE];
            vector<Vertex*> *v1 = keyframes[(kfIdx+0) % KF_BUF_SIZE];
            vector<Vertex*> *v2 = keyframes[(kfIdx+1) % KF_BUF_SIZE];
            vector<Vertex*> *v3 = keyframes[(kfIdx+2) % KF_BUF_SIZE];
            // Use vectors and matrix to compute the interpolated vertices
            computeFrame(uB, v0, v1, v2, v3, vOut);

            // Write this to a new file if it's not a keyframe
            if (frame != keyframeIdx[kfIdx]) {
                int idx = 0;
                while (idx < MAX_THREADS) {
                    if (running[idx]) {
                        idx = (idx+1) % MAX_THREADS;
                    } else {
                        running[idx] = true;
                        threads.push_back(new thread(outputObj, frame, vOut, &running[idx]));
                        idx = MAX_THREADS;
                    }
                }
            }

            // Advance
            frame++;
        }
        if (threads[0])
            threads[0]->join();
        kfIdx++;
    }

    // Join all threads so we can exit safely
    for (int i = 1; i < threads.size(); ++i)
        threads[i]->join();
}

void readObjFile(string *fn, vector<Vertex*> *vertices, bool getFaces) {
    assert(vertices);

    // Open the file
    ifstream inFile(*fn);
    if (!inFile.is_open()) {
        cout << "unable to open " << *fn << endl;
        exit(1);
    }
    // Refresh vertices vector
    clearVertices(vertices);

    // Filler because vertices are 1-indexed
    vertices->push_back(NULL);

    // The string buffer that will be read
    string line;
    // Read each line and create an element
    while (getline(inFile, line)) {
        // Split the space-separated line into a vector of strings
        vector<string> l;
        string temp;
        stringstream s(line);
        while (s >> temp)
            l.push_back(temp);

        // Handle the line according to its identifier (vertex or facet)
        if (l[0] == "v")
            vertices->push_back(new Vertex(stof(l[1]), stof(l[2]), stof(l[3])));
        else if (l[0] == "f" and getFaces)
            faces.push_back(make_tuple(stoi(l[1]), stoi(l[2]), stoi(l[3])));
        else
            break;
    }
    inFile.close();
}

int main(int argc, char **argv) {
    if (argc != 1) {
        cout << "usage: ./interpolate < [file]" << endl;
        exit(1);
    }
    init();
    interpolate();
    cleanup();
    return 0;
}
