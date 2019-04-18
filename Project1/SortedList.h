#pragma once

#ifndef _SORTEDLIST_H
#define _SORTEDLIST_H

using namespace std;
/**
*	array based simple sorted list.
*/
template <class T>
class SortedList {
public:
	/**
	*	default constructor.
	*/
	SortedList() {
		m_Length = 0;
		m_Array = new T[0];
	}

	/**
	*	default copy constructor.
	*/
	SortedList(const SortedList& sortedList) {
		operator=(sortedList);
	}

	/**
	*	destructor.
	*/
	~SortedList() {
		if (m_Length)
			delete[] m_Array;
	}

	/**
	*	= operator.
	*/
	SortedList& operator= (const SortedList& data) {
		if (this != &data) {
			if (m_Array)
				delete[] m_Array;

			m_Length = data.GetLength();
			m_Array = new T[m_Length];

			for (int i = 0; i < data.GetLength(); i++) {
				this->m_Array[i] = data.GetArray()[i];
			}
		}

		return *this;
	}

	/**
	*	= operator.
	*/
	bool operator== (const SortedList& data) {
		for (int i = 0; i < m_Length; i++) {
			if (m_Array[i] != data[i]) {
				return false;
			}
		}

		return true;
	};

	/**
	*	@brief	Output stream에서 SortedList를 적절하게 출력한다.
	*	@pre	SortedList가 정의되어 있어야 함.
	*	@post	SortedList가 출력됨.
	*	@param	out:	출력할 스트림
	*	@param	data:	출력할 SortedList
	*/
	friend std::ostream& operator<< (std::ostream &out, const SortedList &data) {
		if (data.GetLength() == 0) {
			cout << "\tList is empty!" << endl;
			return out;
		}

		cout << "\t[SortedList]\t" << endl;

		for (int i = 0; i < data.GetLength(); i++) {
			cout << data.GetArray()[i] << endl;
		}

		return out;
	}

	/**
	*	@brief	Make list empty.
	*	@pre	none.
	*	@post	clear list.
	*/
	void MakeEmpty();

	/**
	*	@brief	Get a number of records in current list.
	*	@pre	none.
	*	@post	none.
	*	@return	number of records in current list.
	*/
	int GetLength() const{
		return m_Length;
	}

	/**
	*	@brief	Get records in current list.
	*	@pre	none.
	*	@post	none.
	*	@return	Records in current list.
	*/
	T* GetArray() const{
		return m_Array; 
	}

	/**
	*	@brief	primary key를 기준으로 정렬이 되도록 알맞은 자리에 항목을 추가한다.
	*	@pre	list should be initialized.
	*	@post	added the new record into the list.
	*	@param	data: new unspecified class data
	*	@return	return 1 if this function works well, otherwise 0.
	*/
	int Add(T data);

	/**
	*	@brief	찾고자 하는 이름을 포함한 모든 폴더를 검색한다.
	*	@pre	data의 Name은 채워져있어야 한다.
	*	@post	검색된 모든 폴더가 출력된다.
	*	@param	data: 일부만 채워져있는 unspecified class.
	*	@return	찾으면 1, 찾지 못하면 0을 리턴한다.
	*/
	int Get(T& data);
	
	/**
	*	@brief	일부만 채워져있는 data에 알맞은 정보를 저장한다. 대상을 찾을때 Binary Search를 이용한다.
	*	@pre	data의 ID 변수는 채워져있어야 한다.
	*	@post	data 안의 멤버 변수가 모두 채워진다.
	*	@param	data: 일부만 채워져있는 unspecified class.
	*	@return	찾으면 위치를, 찾지 못하면 -1을 리턴한다.
	*/
	int GetBinary(T& data);

	/**
	*	@brief	사용자가 지정한 unspecified class를 배열에서 삭제한다.
	*	@pre	data의 ID 변수는 채워져있어야 한다.
	*	@post	사용자가 입력한 항목은 삭제되고 length가 1줄어든다.
	*	@param	data: 일부만 채워져있는 unspecified class.
	*	@return 삭제에 성공했으면 1, 아니면 0을 리턴한다.
	*/
	int Delete(T data);

	/**
	*	@brief	사용자가 지정한 unspecified class에 해당하는 배열의 자리에 파라미터의 내용을 복사한다.
	*	@pre	data의 모든 변수가 채워져있어야 한다.
	*	@post	사용자가 지정한 항목이 data와 같아진다.
	*	@param	data: 모든 변수가 채워져있는 unspecified class.
	*	@return	성공적으로 수정했으면 1, 아니면 0을 리턴한다.
	*/
	int Replace(T data);

	/**
	*	@brief	파일로부터 데이터를 읽어오는 함수
	*	@pre	none.
	*	@post	none.
	*	@param	fin: 읽을 파일
	*	@param	n: 정보를 읽을 멤버 수
	*	@return	성공적으로 읽었다면 1, 아니면 0을 리턴한다.
	*/
	int ReadDataFromFile(ifstream& fin, int n);

	/**
	*	@brief	파일에 데이터를 저장하는 함수
	*	@pre	none.
	*	@post	none.
	*	@param	fout: 데이터를 쓸 파일
	*	@return	성공적으로 썼다면 1, 아니면 0을 리턴한다.
	*/
	int WriteDataToFile(ofstream& fout);

private:
	T* m_Array;			///< list array.
	int m_Length;		///< number of elements in list.
};

// Make list empty.
template <class T>
void SortedList<T>::MakeEmpty() {
	m_Length = 0;
}

// add a new data into list.
template <class T>
int SortedList<T>::Add(T inData) {
	for (int i = 0; i < m_Length; i++) {
		if (m_Array[i] == inData) {
			return 0;
		}
	}

	// temp를 확장한 크기의 배열로 생성하고, m_Array의 데이터를 모두 옮겨준다.
	T* temp = new T[m_Length + 1];
	for (int i = 0; i < m_Length; i++) {
		temp[i] = m_Array[i];
	}

	// 기존의 m_Array를 지우고, temp로 설정해준다.
	delete[] m_Array;
	m_Array = temp;

	for (int i = 0; i <= m_Length; i++) {
		if (m_Array[i] > inData || i == m_Length) {	//만약 m_Array[i] > inData일 경우 혹은 배열의 마지막이라서 비교 대상이 없는 경우
			for (int j = m_Length; j > i; j--)		//맨 뒤에서 부터 하나씩 당긴다.
				m_Array[j] = m_Array[j - 1];		//배열 밀기
			m_Array[i] = inData;					//배열을 밀은 후 현재 포인터에 아이템 넣는다.
			m_Length++;
			return 1;
		}
	}

	return 1;
}

// Print all members if matched data found
template <class T>
int SortedList<T>::Get(T& data) {
	bool found = false;

	for (int i = 0; i < m_Length; i++) {
		if (m_Array[i].GetName().find(data.GetName()) != string::npos) {
			cout << m_Array[i] << endl;
			found = true;
		}
	}

	if (found) {
		return 1;
	} else {
		return 0;
	}
}

// Return position of array if matched data found
template <class T>
int SortedList<T>::GetBinary(T& data) {
	if (m_Length == 0)				//만약 배열이 비었다면
		return -1;					//실패(0)을 리턴

	int currentPos = m_Length / 2;	//배열의 중간부터 시작
	while (1) {
		if (m_Array[currentPos] == data) {				//현재 아이템과 입력 아이템의 id가 일치한다면
			data = m_Array[currentPos];					//data에 발견한 값을 리턴
			return currentPos;							//성공(1)을 리턴
		} else if (m_Array[currentPos] > data) {		//현재 아이템의 id가 입력 아이템의 id보다 크다면
			if (currentPos == 0)						//처음 배열의 값보다 작은경우
				return -1;								//실패(0)을 리턴
			currentPos /= 2;							//더 작은쪽 인덱스의 절반로 이동
		} else {										//현재 아이템의 id가 입력 아이템의 id보다 작다면
			if (currentPos == m_Length - 1)				//마지막 배열의 값보다 큰경우
				return -1;								//실패(0)을 리턴
			currentPos = (currentPos + m_Length) / 2;	//더 큰쪽 인덱스의 절반으로 이동
		}
	}
	return -1;
}

// Delete data based on folder name
template <class T>
int SortedList<T>::Delete(T data) {
	int pos = GetBinary(data);

	if (pos != -1) {							//id가 일치하는 item을 발견한다면(1)
		for (int i = pos; i < m_Length - 1; i++)//현재 포인터부터 입력된 배열 끝까지 반복
			m_Array[i] = m_Array[i + 1];		//배열 뒤에걸 앞으로 하나씩 땡김
		m_Length--;								//아이템 개수를 하나 줄임
		return 1;								//성공(1)을 리턴
	}

	return 0;	//id가 일치하는 item을 찾지 못한다면 실패(0)을 리턴
}

// Replace data based on folder name
template <class T>
int SortedList<T>::Replace(T data) {
	int pos = GetBinary(data);

	if (pos != -1) {			//일치하는 아이템을 찾은 경우
		m_Array[pos] = data;	//배열에서 현재포인터가 가르키는 것을 백업했던 아이템으로 교체시킴
		return 1;				//성공(1)을 리턴
	}

	return 0;				//id 일치하는 item을 찾지 못한다면 실패(0)을 리턴
}

// Read array from file
template <class T>
int SortedList<T>::ReadDataFromFile(ifstream& fin, int n) {
	return 1;
}

// Write array to file
template <class T>
int SortedList<T>::WriteDataToFile(ofstream& fout) {
	return 1;
}

#endif //_SORTEDLIST_H