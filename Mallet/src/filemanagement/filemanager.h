#pragma once

inline const char rawMapExtension[] = "rmap";
inline const char fileSignature[] = "rmap";

typedef unsigned int version_t;
inline constexpr version_t rmapVersion = 0;

class FileManager
{
public:
	void SaveAsDialog();
	void OpenDialog();

	void SaveToPath(const char*);
	void OpenPath(const char*);

	template<class T>
	inline void Write(std::ofstream& writer, const T& t)
	{
		writer.write(reinterpret_cast<const char*>(&t), sizeof(t));
	}

	inline version_t ReadVersion(std::ifstream& reader)
	{
		char version[sizeof(version_t)];
		reader.read(version, sizeof(version_t));
		return *reinterpret_cast<version_t*>(version);
	}
}; inline FileManager fileManager;