#include "VolumeData.h"

std::unordered_set<std::string> const kAlias::Raw({"raw-file", "Rawfile"});
std::unordered_set<std::string> const kAlias::Resolution({"resolution", "Resolution"});
std::unordered_set<std::string> const kAlias::VolumeSize({"voxel-size", "VoxelSize", "ratio", "Ratio"});
std::unordered_set<std::string> const kAlias::vDataType({"sample-type", "SampleType"});
std::unordered_set<std::string> const kAlias::Endian({"endian", "Endian"});
std::string const InfoKey::Inf = "inf-file";
std::string const InfoKey::Raw = "raw-file";
std::string const InfoKey::Resolution = "resolution";
std::string const InfoKey::VolumeSize = "voxel-size";
std::string const InfoKey::vDataType = "sample-type";
std::string const InfoKey::Endian = "endian";

struct kAlias static const key_alias;
struct InfoKey static const key;

VolumeData::VolumeData(std::string const &filePath, std::string const &infFile) 
    :resolution(0), ratio(0) {
    info[key.Inf] = infFile;
    info["path"] = filePath;

    //read info file
    std::ifstream  infIn(info["path"] + info[key.Inf]);
    if(!infIn) {
        std::cerr << "fail in opening " << info[key.Inf] << " for reading.\n";
        exit(1);
    }

    std::string line;
    while(std::getline(infIn, line)) {
        std::vector<std::string> words = strUtil::split(line, "=");
        
        if (key_alias.Raw.count(words[0])) {
            info[key.Raw] = words[1];
        } else if (key_alias.Resolution.count(words[0])) {
            info[key.Resolution] = words[1];
        } else if (key_alias.vDataType.count(words[0])) {
            info[key.vDataType] = strUtil::remove(words[1], " ", LOWER);
        } else if (key_alias.VolumeSize.count(words[0])) {
            info[key.VolumeSize] = words[1];
        } else if (key_alias.Endian.count(words[0])) {
            info[key.Endian] = strUtil::remove(words[1], "", LOWER);
        }
    }

    if (!info.count(key.Resolution)) {
        std::cerr << "[ERROR] .inf file: Missing attribute \"sample-type\"\n";
        exit(1);
    }
    if (!info.count(key.vDataType)) {
        std::cerr << "[ERROR] .inf file: Missing attribute \"sample-type\"\n";
        exit(1);
    }

    // assign default
    if (!info.count(key.VolumeSize)) {
        info[key.VolumeSize] = "1:1:1";
    }
    if (!info.count(key.Endian)) {
        info[key.Endian] = "little";
    }
    if (!info.count(key.Raw)) {
        auto fileInfo = strUtil::split(infFile, ".");
        info[key.Raw] = fileInfo[0] + ".raw";
    }
    infIn.close();
}

void VolumeData::Read() {
    std::vector<std::string> delimiters({":", "x"});
    std::vector<std::string> strRes;
    for(int i = 0; i < delimiters.size(); ++i) {
        strRes = strUtil::split(info[key.Resolution], delimiters[i]);
        if(strRes.size() != 1)  break;
        strRes.clear();
    }
    for(int i = 0; i < strRes.size(); ++i) {
        resolution[i] = std::stoi(strRes[i]);
    }

    std::vector<std::string> strVsize;
    for(int i = 0; i < delimiters.size(); ++i) {
        strVsize = strUtil::split(info[key.VolumeSize], delimiters[i]);
        if(strRes.size() != 1)  break;
        strVsize.clear();
    }
    for(int i = 0; i < strVsize.size(); ++i) {
        ratio[i] = std::stof(strVsize[i]);
    }
    
    std::ifstream rawIn(info["path"] + info[key.Raw], std::ios::binary);
    if (!rawIn) {
        std::cout << "fail in opening " << info[key.Raw] << " for reading.\n";
        exit(1);
    }

    if(info[key.vDataType] == "unsignedchar") {
        assert(resolution.x > 0 && resolution.y > 0 && resolution.z > 0);
        std::vector<uint8_t> buffer(resolution.x * resolution.y * resolution.z, 0);
        rawIn.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        scalar_field.assign(buffer.begin(), buffer.end());
    } else {
        std::cerr << "not supported\n";
        exit(1);
    }
    rawIn.close();
}

float VolumeData::Value(int x, int y, int z) {
    int idx = x + y * resolution.x + z * resolution.x * resolution.y;
    return scalar_field[idx];
}

float VolumeData::Value(glm::ivec3 coordinate) {
    int idx = coordinate.x + coordinate.y * resolution.x + coordinate.z * resolution.x * resolution.y;
    return scalar_field[idx];
}

float VolumeData::getMaxVal() {
    return *std::max_element(scalar_field.begin(), scalar_field.end());
}

float VolumeData::Value(int idx) {
    if(idx < 0 || idx >= scalar_field.size()) {
        std::cerr << "scalar_field index out of range\n";
        exit(1);
    }
    return scalar_field[idx];
}