// Напишите ответ здесь: 271

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;


string ReadLine() {
	string s;
	getline(cin, s);
	return s;
}

int ReadLineWithNumber() {
	int result;
	cin >> result;
	ReadLine();
	return result;
}

vector<string> SplitIntoWords(const string& text) {
	vector<string> words;
	string word;
	for (const char& c : text) {
		if (c == ' ') {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		}
		else {
			word += c;
		}
	}
	if (!word.empty())
		words.push_back(word);
	return words;
}

struct Query {
	set<string> plus_words;
	set<string> minus_words;
};

struct Document {
	int id;
	double relevance;
};

class SearchServer {
public:
	void SetStopWords(const string& text) {
		for (const string& word : SplitIntoWords(text)) {
			stop_words_.insert(word);
		}
	}

	void AddDocument(int document_id, const string& document) {
		const vector<string> words = SplitIntoWordsNoStop(document);
		double frequency = 1.0 / words.size();
		for (const string& word : words) {
			//добавляем tf
			word_to_document_freqs_[word][document_id] += frequency;
		}
		documents_count_++;
	}

	vector<Document> FindTopDocuments(const string& raw_query) const
	{
		Query query_words = ParseQuery(raw_query);
		vector<Document> result = FindAllDocuments(query_words);
		sort(result.begin(), result.end(), [](const Document& lhs, const Document& rhs)
			{
				return lhs.relevance > rhs.relevance;
			});
		if (result.size() > MAX_RESULT_DOCUMENT_COUNT)
			result.resize(MAX_RESULT_DOCUMENT_COUNT);
		return result;
	}
private:
	Query ParseQuery(const string& text) const {
		Query query_words;
		for (string& word : SplitIntoWordsNoStop(text))
			if (word[0] == '-') {
				word = word.substr(1);
				query_words.minus_words.insert(word);
			}
			else {
				query_words.plus_words.insert(word);
			}
		return query_words;
	}
	double ComputeIDF(const string& word) const {
		double idf = log(static_cast<double>(documents_count_) / word_to_document_freqs_.at(word).size());
		return idf;
	}
	vector<Document> FindAllDocuments(const Query& query_words) const {
		double idf = 0;
		map<int, double> document_to_relevance;
		for (const string& word : query_words.plus_words) {
			if (word_to_document_freqs_.count(word) != 0) {

				idf = ComputeIDF(word);
				for (const auto& [document_id, tf] : word_to_document_freqs_.at(word)) {
					document_to_relevance[document_id] += tf * idf;
				}
			}
		}
		for (const string& word : query_words.minus_words) {
			if (word_to_document_freqs_.count(word) != 0) {

				for (const auto& [document_id, tf] : word_to_document_freqs_.at(word)) {
					document_to_relevance.erase(document_id);
				}
			}
		}

		vector<Document> result;
		for (const auto& [document_id, relevance] : document_to_relevance)
			result.push_back({ document_id, relevance });
		return result;

	}
	bool IsStopWord(const string& word) const {
		return stop_words_.count(word) > 0;
	}
	vector<string> SplitIntoWordsNoStop(const string& text) const {
		vector<string> words;
		for (const string& word : SplitIntoWords(text)) {
			if (!IsStopWord(word)) {
				words.push_back(word);
			}
		}
		return words;
	}

	int documents_count_ = 0;
	set<string> stop_words_;
	map<string, map<int, double>> word_to_document_freqs_;
	map<string, set<int>> word_to_documents_;
};
SearchServer CreateSearchServer() {
	SearchServer search_server;
	search_server.SetStopWords(ReadLine());

	const int document_count = ReadLineWithNumber();
	for (int document_id = 0; document_id < document_count; ++document_id) {
		search_server.AddDocument(document_id, ReadLine());
	}

	return search_server;
}
int main() {
	const SearchServer search_server = CreateSearchServer();

	const string query = ReadLine();
	for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
		cout << "{ document_id = "s << document_id << ", "
			<< "relevance = "s << relevance << " }"s << endl;
	}
}