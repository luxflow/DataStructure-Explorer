﻿#include "Application.h"

// 새로운 폴더를 생성함.
void Application::NewFolder(const JSObject& thisObject, const JSArgs& args) {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FolderType temp;
	temp.SetLocation(location);
	temp.SetName(std::string(((String)args[0]).utf8().data()));
	temp.SetParent(m_CurFolder);

	if (m_CurFolder->GetSubFolderList()->Add(temp)) {
		m_CurFolder->SetModifyDateToNow();
		m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() + 1);

		// Create new folder in Windows
		if (Windows::CreateDirectoryWithPath(location + temp.GetName())) {
			if (UpdateCurrentFolderObject.IsValid()) {
				JSObject jso = m_CurFolder->to_jsobject();
				UpdateCurrentFolderObject(jso, { 0 });
			}
		}
	}
}

// 폴더 삭제
void Application::DeleteFolder(const JSObject& thisObject, const JSArgs& args) {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FolderType temp;
	temp.SetPropertyFromKB(location);

	if (m_CurFolder->GetSubFolderList()->Delete(temp)) {
		m_CurFolder->SetModifyDateToNow();
		m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() - 1);

		// Delete folder in Windows
		if (!Windows::DeleteDirectoryWithPath(location, temp.GetName())) {
			cout << "\tFail to delete directory in Windows!" << endl;
			system("pause");
		}
	}
}

// Rename folder
void Application::RenameFolder(const JSObject& thisObject, const JSArgs& args) {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	cout << "\t바꾸고 싶은 폴더 이름을 입력해주세요." << endl;
	FolderType* temp = new FolderType();
	temp->SetPropertyFromKB(location);

	if (m_CurFolder->GetSubFolderList()->GetBinary(*temp) != -1) {
		cout << endl << "\t무엇으로 바꾸시겠습니까?" << endl;
		FolderType *renew = new FolderType();
		renew->SetPropertyFromKB(location);

		if (m_CurFolder->GetSubFolderList()->GetBinary(*renew) == -1) {
			if (m_CurFolder->GetSubFolderList()->Delete(*temp)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() - 1);
			} else {
				cout << "\t폴더명 변경 실패!" << endl;
				system("pause");
				return;
			}

			// Rename folder in Windows
			Windows::RenameDirectoryWithPath(location, temp->GetName(), renew->GetName());
			
			temp->SetAccessDateToNow();
			temp->SetName(renew->GetName());
			
			if (m_CurFolder->GetSubFolderList()->Add(*temp)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() + 1);
				RecursiveUpdateLocation(m_CurFolder, location);
			} else {
				cout << "\t폴더명 변경 실패!" << endl;
				system("pause");
				return;
			}
		} else {
			cout << "\t폴더명 중복입니다." << endl;
			system("pause");
		}
	}
}

// 서브 폴더 열기
void Application::OpenFolder(const JSObject& thisObject, const JSArgs& args) {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FolderType temp;
	temp.SetPropertyFromKB(location);

	int n = m_CurFolder->GetSubFolderList()->GetBinary(temp);

	if (n == -1) {
		cout << "해당 폴더가 존재하지 않습니다!" << endl;
		system("pause");
		return;
	}

	FolderType& result = (m_CurFolder->GetSubFolderList()->GetArray())[n];
	result.SetParent(m_CurFolder);

	m_RecentFolder.EnQueue(result);
	m_FrequentFolder.AddKey(result);
	m_FrequentFolder.count[m_FrequentFolder.GetIndexOfKey(result)]++;

	m_CurFolder = &result;
	m_CurFolder->SetAccessDateToNow();
}

// Set as favorite folder
void Application::SetAsFavoriteFolder() {
	FolderType* temp = new FolderType(*m_CurFolder);
	m_FavoriteFolder.EnQueue(*temp);
}

// 최근 열어본 폴더 출력
void Application::DisplayRecentFolder() {
	cout << "\t=================" << endl;
	cout << "\t[Recent Folder]" << endl;
	cout << m_RecentFolder << endl;
}

// 좋아하는 폴더 출력
void Application::DisplayFavoriteFolder() {
	cout << "\t=================" << endl;
	cout << "\t[Favorite Folder]" << endl;
	cout << m_FavoriteFolder << endl;
}

// 자주 사용하는 폴더 출력
void Application::DisplayFrequentFolder() {
	cout << "\t=================" << endl;
	cout << "\t[Frequent Folder]" << endl;
	cout << m_FrequentFolder << endl;
}

// 폴더 정보 출력
void Application::DisplayProperty() {
	cout << "\t==============" << endl;
	cout << "\t현재 폴더 정보" << endl;
	cout << *m_CurFolder << endl;

	if (m_CurFolder->GetFolderNumber() > 0) {
		cout << "\t----------------" << endl;
		cout << "\t서브 폴더 리스트 (" << m_CurFolder->GetFolderNumber() << "개)" << endl;
		cout << *m_CurFolder->GetSubFolderList();
	}

	if (m_CurFolder->GetFileNumber() > 0) {
		cout << "\t----------------" << endl;
		cout << "\t파일 리스트 (" << m_CurFolder->GetFileNumber() << "개)" << endl;
		cout << *m_CurFolder->GetFileList();
	}

	if (m_CopyFolder->GetLength() > 0 || m_CutFolder->GetLength() > 0) {
		cout << "\t----------------" << endl << "\t";
		cout << m_CopyFolder->GetLength() << "개의 복사된 폴더, "
			<< m_CutFolder->GetLength() << "개의 잘라낸 폴더가 있습니다." << endl;
	}

	if (m_CopyFile->GetLength() > 0 || m_CutFile->GetLength() > 0) {
		cout << "\t----------------" << endl << "\t";
		cout << m_CopyFile->GetLength() << "개의 복사된 파일, "
			<< m_CutFile->GetLength() << "개의 잘라낸 파일이 있습니다." << endl;
	}
}

// 상위 폴더로 이동
void Application::MoveToParentFolder() {
	if (m_CurFolder->GetParent() == nullptr) {
		m_RecentFolder.EnQueue(m_RootFolder);
		m_RootFolder.SetAccessDateToNow();
		m_FrequentFolder.AddKey(m_RootFolder);
		m_FrequentFolder.count[m_FrequentFolder.GetIndexOfKey(m_RootFolder)]++;
		m_CurFolder = &m_RootFolder;
	} else {
		m_RecentFolder.EnQueue(*(m_CurFolder->GetParent()));
		m_CurFolder->GetParent()->SetAccessDateToNow();
		m_FrequentFolder.AddKey(*m_CurFolder->GetParent());
		m_FrequentFolder.count[m_FrequentFolder.GetIndexOfKey(*m_CurFolder->GetParent())]++;
		m_CurFolder = m_CurFolder->GetParent();
	}
}

// 폴더 복사
void Application::CopyFolder() {
	FolderType *temp = new FolderType(*m_CurFolder);
	
	// 이미 복사하고자 하는 폴더 리스트에 존재하면 실행을 종료
	if (m_CopyFolder->GetBinary(*temp) != -1) return;
	
	if (m_CutFolder->GetBinary(*temp) == -1) {
		m_CopyFolder->Add(*temp);
	} else {
		// 자를 폴더 리스트에 이미 있으면, 복사 폴더 리스트로 옮기기
		m_CutFolder->Delete(*temp);
		m_CopyFolder->Add(*temp);
	}

	MoveToParentFolder();
}

// 폴더 자르기
void Application::CutFolder() {
	FolderType *temp = new FolderType(*m_CurFolder);

	// 이미 자르고자 하는 폴더 리스트에 존재하면 실행을 종료
	if (m_CutFolder->GetBinary(*temp) != -1) return;

	if (m_CopyFolder->GetBinary(*temp) == -1) {
		m_CutFolder->Add(*temp);
	} else {
		// 복사 폴더 리스트에 이미 있으면, 자를 폴더 리스트로 옮기기
		m_CopyFolder->Delete(*temp);
		m_CutFolder->Add(*temp);
	}

	MoveToParentFolder();
}

// 복사한 폴더 붙여넣기
void Application::PasteCopyFolder() {
	int i, j;
	FolderType* temp;

	for (i = 0; i < m_CopyFolder->GetLength(); i++) {
		temp = new FolderType(m_CopyFolder->GetArray()[i]);

		for (j = 0; j < m_CurFolder->GetFolderNumber(); j++) {
			// 붙여넣기 할 폴더에 중복되는 이름을 가진 폴더가 있는지 확인한다.
			if ((m_CurFolder->GetSubFolderList()->GetArray())[j].GetName()
				== temp->GetName()) {
				break;
			}
		}

		if (j == m_CurFolder->GetFolderNumber()) {
			string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";

			// Copy and paste in Windows
			Windows::CopyDirectoryWithPath(temp->GetLocation(), temp->GetName(), location);

			// 중복되는 이름을 가진 폴더가 없다면, 붙여넣는다.
			temp->SetParent(m_CurFolder);
			temp->SetLocation(location);

			if (m_CurFolder->GetSubFolderList()->Add(*temp)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() + 1);
				RecursiveUpdateLocation(m_CurFolder, location);
			}

			cout << "\tFolder " << temp->GetName() << " is successfully copied and pasted!" << endl;
		} else {
			// 중복되는 이름을 가진 폴더가 있다면, 에러를 띄우고 스킵한다.
			cout << "\tFolder " << temp->GetName() << " is duplicated, program will skip this folder!" << endl;
		}
	}

	delete m_CopyFolder;
	m_CopyFolder = new SortedList<FolderType>();
}

// 잘라낸 폴더 붙여넣기
void Application::PasteCutFolder() {
	int i, j;
	FolderType* temp, *copy;

	for (i = 0; i < m_CutFolder->GetLength(); i++) {
		temp = new FolderType(m_CutFolder->GetArray()[i]);

		for (j = 0; j < m_CurFolder->GetFolderNumber(); j++) {
			// 붙여넣기 할 폴더에 중복되는 이름을 가진 폴더가 있는지 확인한다.
			if ((m_CurFolder->GetSubFolderList()->GetArray())[j].GetName()
				== temp->GetName()) {
				break;
			}
		}

		if (j == m_CurFolder->GetFolderNumber()) {
			string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
			// Cut and paste in Windows
			Windows::CopyDirectoryWithPath(temp->GetLocation(), temp->GetName(), location);
			Windows::DeleteDirectoryWithPath(temp->GetLocation(), temp->GetName());

			copy = new FolderType(*temp);

			// 중복되는 이름을 가진 폴더가 없다면, 붙여넣는다.
			copy->SetParent(m_CurFolder);
			copy->SetLocation(location);

			if (m_CurFolder->GetSubFolderList()->Add(*copy)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFolderNumber(m_CurFolder->GetFolderNumber() + 1);
				RecursiveUpdateLocation(m_CurFolder, location);
			}
			delete copy;

			if (j == m_CurFolder->GetFolderNumber()) {
				// 복사와는 다르게, 자르기에선 원본 폴더에서 해당 폴더를 삭제한다.
				FolderType child = m_CutFolder->GetArray()[i];
				FolderType parent = *m_CutFolder->GetArray()[i].GetParent();

				if (parent.GetSubFolderList()->Delete(child)) {
					parent.SetModifyDateToNow();
					parent.SetFolderNumber(parent.GetFolderNumber() - 1);
				}
			}

			cout << "\tFolder " << temp->GetName() << " is successfully cutted and pasted!" << endl;
		} else {
			// 중복되는 이름을 가진 폴더가 있다면, 에러를 띄우고 스킵한다.
			cout << "\tFolder " << temp->GetName() << " is duplicated, program will skip this folder!" << endl;
		}
	}

	delete m_CutFolder;
	m_CutFolder = new SortedList<FolderType>();
}

void Application::RecursiveUpdateLocation(FolderType *list, string location) {
	for (int i = 0; i < list->GetFileNumber(); i++) {
		SortedList<FileType>* flist = list->GetFileList();

		if (flist->GetArray()[i].GetLocation() != location) {
			flist->GetArray()[i].SetLocation(location);
		}
	}

	for (int i = 0; i < list->GetFolderNumber(); i++) {
		SortedList<FolderType>* dlist = list->GetSubFolderList();

		if (dlist->GetArray()[i].GetLocation() != location) {
			dlist->GetArray()[i].SetLocation(location);
		}

		if (dlist->GetArray()[i].GetFolderNumber() > 0
			|| dlist->GetArray()[i].GetFileNumber() > 0) {
			RecursiveUpdateLocation(&dlist->GetArray()[i], location + dlist->GetArray()[i].GetName() + "\\");
		}
	}
}

// 폴더 붙여넣기
void Application::PasteFolder() {
	PasteCopyFolder();
	PasteCutFolder();
	system("pause");
}

// Add new file
void Application::NewFile(const JSObject& thisObject, const JSArgs& args) {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FileType temp;
	temp.SetLocation(location);
	temp.SetName(std::string(((String)args[0]).utf8().data()));
	temp.SetExtension(std::string(((String)args[1]).utf8().data()));
	temp.SetParent(m_CurFolder);

	if (m_CurFolder->GetFileList()->Add(temp)) {
		m_CurFolder->SetModifyDateToNow();
		m_CurFolder->SetFileNumber(m_CurFolder->GetFileNumber() + 1);
		
		// Create new file in Windows
		if (Windows::CreateFileWithPath(location, temp.GetName(), temp.GetExtension())) {
			if (UpdateCurrentFolderObject.IsValid()) {
				JSObject jso = m_CurFolder->to_jsobject();
				UpdateCurrentFolderObject(jso, { 0 });
			}
		}
	}
}

// Delete file
void Application::DeleteFileA() {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FileType temp;
	temp.SetPropertyFromKB(location);

	if (m_CurFolder->GetFileList()->Delete(temp)) {
		m_CurFolder->SetModifyDateToNow();
		m_CurFolder->SetFileNumber(m_CurFolder->GetFileNumber() - 1);

		// Delete file in Windows
		if (!Windows::DeleteFileWithPath(location, temp.GetName(), temp.GetExtension())) {
			cout << "\tFail to delete file in Windows!" << endl;
			system("pause");
		}
	}
}

// Rename file
void Application::RenameFile() {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	cout << "\t바꾸고 싶은 파일 이름을 입력해주세요." << endl;
	FileType* temp = new FileType();
	temp->SetPropertyFromKB(location);

	if (m_CurFolder->GetFileList()->GetBinary(*temp) != -1) {
		cout << endl << "\t무엇으로 바꾸시겠습니까?" << endl;
		FileType *renew = new FileType();
		renew->SetPropertyFromKB(location);

		if (m_CurFolder->GetFileList()->GetBinary(*renew) == -1) {
			m_CurFolder->GetFileList()->Delete(*temp);

			// Rename file in Windows
			Windows::RenameFileWithPath(location, temp->GetName(), temp->GetExtension(),
												renew->GetName(), renew->GetExtension());

			temp->SetAccessDateToNow();
			temp->SetName(renew->GetName());
			temp->SetExtension(renew->GetExtension());

			m_CurFolder->GetFileList()->Add(*temp);
		}
		else {
			cout << "\t파일명 중복입니다." << endl;
		}
	}
}

// Open file if exists in folder
void Application::OpenFile() {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FileType *temp = new FileType();
	temp->SetPropertyFromKB(location);

	if (m_CurFolder->GetFileList()->GetBinary(*temp) == -1)
		return;

	m_RecentFile.EnQueue(*temp);
	m_FrequentFile.AddKey(*temp);
	m_FrequentFile.count[m_FrequentFile.GetIndexOfKey(*temp)]++;

	Windows::ExecuteFile(location, temp->GetName(), temp->GetExtension());
}

// Set as favorite file
void Application::SetAsFavoriteFile() {
	string location = m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\";
	FileType* temp = new FileType();
	temp->SetPropertyFromKB(location);

	if (m_CurFolder->GetFileList()->GetBinary(*temp) != -1) {
		m_FavoriteFile.EnQueue(*temp);
	} else {
		cout << "\tThere is no such file." << endl;
		system("pause");
	}
}

// Copy file
void Application::CopyFileA() {
	FileType *temp = new FileType();
	temp->SetPropertyFromKB(m_CurFolder->GetLocation () + m_CurFolder->GetName () + "\\");

	// 복사하고자 하는 파일이 현재 폴더에 존재하지 않으면 실행을 종료
	if (m_CurFolder->GetFileList()->GetBinary(*temp) == -1) return;

	// 이미 복사하고자 하는 파일 리스트에 존재하면 실행을 종료
	if (m_CopyFile->GetBinary(*temp) != -1) return;

	if (m_CutFile->GetBinary(*temp) == -1) {
		m_CopyFile->Add(*temp);
	} else {
		// 자를 파일 리스트에 이미 있으면, 복사 파일 리스트로 옮기기
		m_CutFile->Delete(*temp);
		m_CopyFile->Add(*temp);
	}
}

// Cut file
void Application::CutFile() {
	FileType *temp = new FileType();
	temp->SetPropertyFromKB(m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\");

	// 복사하고자 하는 파일이 현재 폴더에 존재하지 않으면 실행을 종료
	if (m_CurFolder->GetFileList()->GetBinary(*temp) == -1) return;

	// 이미 자르고자 하는 파일 리스트에 존재하면 실행을 종료
	if (m_CutFile->GetBinary(*temp) != -1) return;

	if (m_CopyFile->GetBinary(*temp) == -1) {
		m_CutFile->Add(*temp);
	} else {
		// 복사 파일 리스트에 이미 있으면, 자를 파일 리스트로 옮기기
		m_CopyFile->Delete(*temp);
		m_CutFile->Add(*temp);
	}
}

// Paste copy file
void Application::PasteCopyFile() {
	int i, j;
	FileType* temp;

	for (i = 0; i < m_CopyFile->GetLength(); i++) {
		temp = new FileType(m_CopyFile->GetArray()[i]);

		for (j = 0; j < m_CurFolder->GetFileNumber(); j++) {
			// 붙여넣기 할 폴더에 중복되는 이름을 가진 파일이 있는지 확인한다.
			if ((m_CurFolder->GetFileList()->GetArray())[j].GetName() == temp->GetName()
				&& (m_CurFolder->GetFileList()->GetArray())[j].GetExtension() == temp->GetExtension()) {
				break;
			}
		}

		if (j == m_CurFolder->GetFileNumber()) {
			// Copy and paste in Windows
			Windows::CopyFileWithPath(temp->GetLocation(), temp->GetName(), temp->GetExtension(),
									  m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\");

			// 중복되는 이름을 가진 파일이 없다면, 붙여넣는다.
			temp->SetLocation(m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\");

			if (m_CurFolder->GetFileList()->Add(*temp)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFileNumber(m_CurFolder->GetFileNumber() + 1);
			}

			cout << "\tFile " << temp->GetName() << "." << temp->GetExtension()
				<< " is successfully copied and pasted!" << endl;
		} else {
			// 중복되는 이름을 가진 파일이 있다면, 에러를 띄우고 스킵한다.
			cout << "\tFile " << temp->GetName() << "." << temp->GetExtension()
				<< " is duplicated, program will skip this file!" << endl;
		}
	}
	delete m_CopyFile;
	m_CopyFile = new SortedList<FileType>();
}

// Paste cut file
void Application::PasteCutFile() {
	int i, j;
	FileType* temp;

	for (i = 0; i < m_CutFile->GetLength(); i++) {
		temp = new FileType(m_CutFile->GetArray()[i]);

		for (j = 0; j < m_CurFolder->GetFileNumber(); j++) {
			// 붙여넣기 할 폴더에 중복되는 이름을 가진 파일이 있는지 확인한다.
			if ((m_CurFolder->GetFileList()->GetArray())[j].GetName() == temp->GetName()
				&& (m_CurFolder->GetFileList()->GetArray())[j].GetExtension() == temp->GetExtension()) {
				break;
			}
		}

		if (j == m_CurFolder->GetFileNumber()) {
			// Cut and paste in Windows
			Windows::CopyFileWithPath(temp->GetLocation(), temp->GetName(), temp->GetExtension(),
									  m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\");
			Windows::DeleteFileWithPath(temp->GetLocation(), temp->GetName(), temp->GetExtension());

			// 복사와는 다르게, 자르기에선 원본 폴더에서 해당 파일을 삭제한다.
			if (temp->GetParent()->GetFileList()->Delete(*temp)) {
				temp->GetParent()->SetModifyDateToNow();
				temp->GetParent()->SetFileNumber(temp->GetParent()->GetFileNumber() - 1);
			}

			// 중복되는 이름을 가진 파일이 없다면, 붙여넣는다.
			temp->SetParent(m_CurFolder);
			temp->SetLocation(m_CurFolder->GetLocation() + m_CurFolder->GetName() + "\\");

			if (m_CurFolder->GetFileList()->Add(*temp)) {
				m_CurFolder->SetModifyDateToNow();
				m_CurFolder->SetFileNumber(m_CurFolder->GetFileNumber() + 1);
			}

			cout << "\tFile " << temp->GetName() << "." << temp->GetExtension()
				<< " is successfully cutted and pasted!" << endl;
		} else {
			// 중복되는 이름을 가진 파일이 있다면, 에러를 띄우고 스킵한다.
			cout << "\tFile " << temp->GetName() << "." << temp->GetExtension()
				<< " is duplicated, program will skip this file!" << endl;
		}
	}
	delete m_CutFile;
	m_CutFile = new SortedList<FileType>();
}

// Paste file
void Application::PasteFile() {
	PasteCopyFile();
	PasteCutFile();
	system("pause");
}

// Search folder or file in whole system
void Application::Search() {
	string word;
	cout << "\tSearch word? ";
	cin >> word;

	RecursiveSearch(&m_RootFolder, word);
	system("pause");
}

// Temporary function to search recursively
void Application::RecursiveSearch(FolderType* f, string w) {
	FolderType a;
	FileType b;
	int i;
	a.SetName(w), b.SetName(w);

	if(f->GetFolderNumber() > 0) f->GetSubFolderList()->Get(a);
	if(f->GetFileNumber() > 0) f->GetFileList()->Get(b);

	for (i = 0; i < f->GetFolderNumber(); i++) {
		if (f->GetSubFolderList()->GetArray()[i].GetFolderNumber() > 0
			|| f->GetSubFolderList()->GetArray()[i].GetFileNumber() > 0) {
			RecursiveSearch(&f->GetSubFolderList()->GetArray()[i], w);
		}
	}

	return;
}

// 최근 열어본 파일 출력
void Application::DisplayRecentFile() {
	cout << "\t=============" << endl;
	cout << "\t[Recent File]" << endl;
	cout << m_RecentFile << endl;
}

// 좋아하는 파일 출력
void Application::DisplayFavoriteFile() {
	cout << "\t=============" << endl;
	cout << "\t[Favorite File]" << endl;
	cout << m_FavoriteFile << endl;
}

// 자주 사용하는 파일 출력
void Application::DisplayFrequentFile() {
	cout << "\t=============" << endl;
	cout << "\t[Frequent File]" << endl;
	cout << m_FrequentFile << endl;
}

// Display recent folder or file
void Application::DisplayRecent() {
	DisplayRecentFolder();
	DisplayRecentFile();
	system("pause");
}

// Display favorite folder or file
void Application::DisplayFavorite() {
	DisplayFavoriteFolder();
	DisplayFavoriteFile();
	system("pause");
}

// Display Frequent folder or file
void Application::DisplayFrequent() {
	DisplayFrequentFolder();
	DisplayFrequentFile();
	system("pause");
}

// 파일시스템으로부터 구조 읽어들이기
void Application::ReadDataFromSystem() {
	if (Windows::ReadStructureFromSystem(&m_RootFolder)) {
		UpdateCurrentFolderObject(m_RootFolder.to_jsobject(), { 0 });
	}
}
