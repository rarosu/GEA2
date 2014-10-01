#define ALL_PUBLIC

#include <istream>
#include <gtest/gtest.h>
#include <Filesystem.h>
#include <ZipArchive.h>
#include <FilesystemArchive.h>

class FilesystemTest : public ::testing::Test
{
protected:
	Filesystem fs;
};


TEST_F(FilesystemTest, ArchiveTest)
{
	ASSERT_TRUE(fs.AddArchive<ZipArchive>("../Assets/Test.zip"));
	ASSERT_TRUE(fs.AddArchive<ZipArchive>("../Assets/Test2.zip"));

	ASSERT_EQ(fs.archives.size(), 2);
	ASSERT_EQ(fs.files.size(), 7);

	std::vector<std::pair<std::string, std::string>> contents;
	contents.push_back(std::pair<std::string, std::string>("Test1.txt", "Test1 content"));
	contents.push_back(std::pair<std::string, std::string>("Test2.txt", "Test2 content"));
	contents.push_back(std::pair<std::string, std::string>("LONGFUCKINGFILENAMEHERECOURTESYOFDANIEL.txt", "FUCK YEAH!"));
	contents.push_back(std::pair<std::string, std::string>("ThisIsADirectory/ThisIsInADirectory.txt", "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."));
	contents.push_back(std::pair<std::string, std::string>("Kevin.txt", "Kevin kom på den här iden."));
	contents.push_back(std::pair<std::string, std::string>("Thomas.txt", "Om det här inte funkar så är det Kevins fel."));
	contents.push_back(std::pair<std::string, std::string>("Mapp/Daniel.txt", "Uuh, tack."));

	ASSERT_EQ(fs.GetFile("ThisFileDoesNotExist.exe"), nullptr);

	for (size_t i = 0; i < contents.size(); ++i)
	{
		ASSERT_NE(fs.GetFile(contents[i].first), nullptr);

		// Assert that the file exists.
		ASSERT_NE(fs.files[contents[i].first], nullptr);

		// Assert that the contents are loaded properly.
		File* file = fs.files[contents[i].first];
		ASSERT_TRUE(file->Open());

		size_t filesize = file->GetFileSize();
		ASSERT_EQ(filesize, contents[i].second.size());

		char* filecontent = new char[filesize];
		ASSERT_EQ(file->Read(filecontent, filesize), filesize);
		
		CharBuffer buffer(filecontent, filecontent + filesize);
		std::istream stream(&buffer);

		std::string line;
		std::getline(stream, line);

		ASSERT_EQ(line, contents[i].second);

		file->Close();
	}
}

TEST_F(FilesystemTest, DirectoryTest)
{
	fs.AddArchive<FilesystemArchive>("..\\..\\assets\\TestDirectory");
}