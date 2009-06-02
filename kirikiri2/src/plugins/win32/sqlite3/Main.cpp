#include <stdio.h>
#include <process.h>
#include <string>
#include "ncbind/ncbind.hpp"
#include "sqlite3.h"
#include "sqlite3_xp3_vfs/xp3_vfs.h"


// ���b�Z�[�W�R�[�h
#define	WM_SQLITE       (WM_APP+8)
#define	SQLITE_START    0x0100
#define	SQLITE_PROGRESS 0x0200
#define	SQLITE_END      0x0400

/**
 * �X�e�[�g�����g tTJSVariant ���o�C���h
 * @param stmt �X�e�[�g�����g
 * @param variant �o�C���h����f�[�^
 * @param num �o�C���h�ʒu
 */
static void
bind(sqlite3_stmt *stmt, tTJSVariant &variant, int pos)
{
	pos++;
	switch (variant.Type()) {
	case tvtInteger:
		sqlite3_bind_int64(stmt, pos, variant);
		break;
	case tvtReal:
		sqlite3_bind_double(stmt, pos, variant);
		break;
	case tvtString:
		{
			tTJSVariantString *str = variant.AsStringNoAddRef();
			sqlite3_bind_text16(stmt, pos, *str, str->GetLength() * sizeof tjs_char, SQLITE_TRANSIENT);
		}
		break;
	case tvtOctet:
		{
			tTJSVariantOctet *octet = variant.AsOctetNoAddRef();
			sqlite3_bind_blob(stmt, pos, octet->GetData(), octet->GetLength(), SQLITE_STATIC);
		}
		break;
	default:
		sqlite3_bind_null(stmt, pos);
		break;
	}
}

/**
 * �X�e�[�g�����g���� tTJSVariant ���擾
 * @param stmt �X�e�[�g�����g
 * @param variant �i�[��
 * @param num �J�����ԍ�
 */
static void
getColumn(sqlite3_stmt *stmt, tTJSVariant &variant, int num)
{
	switch (sqlite3_column_type(stmt, num)) {
	case SQLITE_INTEGER:
		variant = sqlite3_column_int64(stmt, num);
		break;
	case SQLITE_FLOAT:
		variant = sqlite3_column_double(stmt, num);
		break;
	case SQLITE_TEXT:
		variant = (const tjs_char *)sqlite3_column_text16(stmt, num);
		break;
	case SQLITE_BLOB:
		variant = tTJSVariant((const tjs_uint8 *) sqlite3_column_blob(stmt, num), sqlite3_column_bytes(stmt, num));
		break;
	default:
		variant.Clear();
		break;
	}
}

/**
 * Sqlite�N���X
 */
class Sqlite {

public:
	/**
	 * �R���X�g���N�^
	 * @param database �f�[�^�x�[�X�t�@�C����
	 * @param readonly �ǂݍ��ݐ�p
	 */
	Sqlite(const tjs_char *database, bool readonly=false) : db(NULL) {
		if (readonly) {
			// �ǂݍ��ݐ�p���͋g���g���̏����n���g��
			int	len = ::WideCharToMultiByte(CP_UTF8, 0, database, -1, NULL, 0, NULL, NULL);
			if (len > 0) {
				char *buf = new char[len + 1];
				::WideCharToMultiByte(CP_UTF8, 0, database, -1, buf, len, NULL, NULL);
				buf[len] = '\0';
				sqlite3_open_v2(buf, &db, SQLITE_OPEN_READONLY, "xp3");
				delete[] buf;
			}
		} else {
			// �����łȂ��ꍇ��OS�̏����n���g��
			ttstr filename = database;
			if (*database == '\0' || *database == ':') {
				// �󕶎��܂��� ':' �Ŏn�܂�ꍇ�� sqlite �̓���Ȉ���
				sqlite3_open16(database, &db);
			} else {
				filename = TVPNormalizeStorageName(filename);
				if (filename.length() && !wcschr(filename.c_str(), '>')) {
					TVPGetLocalName(filename);
					sqlite3_open16(filename.c_str(), &db);
				}
			}
		}
	}

	/**
	 * �f�X�g���N�^
	 */
	~Sqlite() {
		if (db) {
			sqlite3_close(db);
		}
	}

	/**
	 * SQL�����s����
	 */
	static tjs_error exec(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, Sqlite *self) {
		if (numparams < 2) {
			return TJS_E_BADPARAMCOUNT;
		}
		sqlite3_stmt *stmt = NULL;
		int ret;
		if ((ret = sqlite3_prepare16_v2(self->db, params[1]->GetString(), -1, &stmt, NULL)) == SQLITE_OK) {
			sqlite3_reset(stmt);
			// �p�����[�^���o�C���h
			for (int n=2;n<numparams;n++) {
				::bind(stmt, *params[n], n-2);
			}
			if (params[0]->Type() == tvtObject) {
				tTJSVariantClosure &callback = params[0]->AsObjectClosureNoAddRef();
				// �J������
				int argc = sqlite3_column_count(stmt);
				// ����������
				tTJSVariant **args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
				}
				while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
					// �����ɓW�J
					for (int i=0;i<argc;i++) {
						::getColumn(stmt, *args[i], i);
					}
					callback.FuncCall(0, NULL, NULL, NULL, argc, args, NULL);
				}
				// �����j��
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			} else {
				while ((ret = sqlite3_step(stmt)) == SQLITE_ROW);
			}
			sqlite3_finalize(stmt);
		}
		if (result) {
			*result = ret == SQLITE_OK || ret == SQLITE_DONE;
		}
		return TJS_S_OK;
	}

	/**
	 * �l�擾�p��SQL�����s����B
	 */
	static tjs_error execValue(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, Sqlite *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		sqlite3_stmt *stmt = NULL;
		if (sqlite3_prepare16_v2(self->db, params[0]->GetString(), -1, &stmt, NULL) == SQLITE_OK) {
			sqlite3_reset(stmt);
			for (int n=1;n<numparams;n++) {
				::bind(stmt, *params[n], n-2);
			}
			int argc = sqlite3_column_count(stmt);
			if (sqlite3_step(stmt) == SQLITE_ROW && argc > 0) {
				if (result) {
					::getColumn(stmt, *result, 0);
				}
			}
			sqlite3_finalize(stmt);
		}
		return TJS_S_OK;
	}
	
	void begin() {
		sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	}
	void commit() {
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
	}
	void rollback() {
		sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
	}

	int getErrorCode() const {
		return db ? sqlite3_errcode(db) : -1;
	}
	
	ttstr getErrorMessage() const {
		return db ? ttstr((const tjs_char*)sqlite3_errmsg16(db)) : ttstr("database open failed");
	}
	
	/**
	 * �C���X�^���X�����t�@�N�g��
	 */
	static tjs_error factory(Sqlite **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		*result = new Sqlite(params[0]->GetString(), numparams > 1 ? (int)*params[1] != 0 : false);
		return S_OK;
	}
	
	sqlite3 *getDatabase() const {
		return db;
	}
	
private:
	sqlite3 *db;
};

NCB_REGISTER_CLASS(Sqlite) {
	Factory(&ClassT::factory);
	RawCallback(TJS_W("exec"), &Class::exec, 0);
	RawCallback(TJS_W("execValue"), &Class::execValue, 0);
	NCB_METHOD(begin);
	NCB_METHOD(commit);
	NCB_METHOD(rollback);
	NCB_PROPERTY_RO(errorCode, getErrorCode);
	NCB_PROPERTY_RO(errorMessage, getErrorMessage);
};

/**
 * Sqlite�̃X�e�[�g�����g�������N���X
 */
class SqliteStatement {

public:
	SqliteStatement(tTJSVariant &sqlite, const tjs_char *sql) : sqlite(sqlite), stmt(NULL) {
		if (sql) {
			open(sql);
		}
	}

	~SqliteStatement() {
		close();
	}

	void open(const tjs_char *sql) {
		close();
		Sqlite *sq = ncbInstanceAdaptor<Sqlite>::GetNativeInstance(sqlite.AsObjectNoAddRef());
		if (sq) {
			sqlite3_prepare16_v2(sq->getDatabase(), sql, -1, &stmt, NULL);
			reset();
		}
	}

	void close() {
		if (stmt) {
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}

	tTJSString getSql() const {
		ttstr ret;
		const char *sql = sqlite3_sql(stmt);
		if (sql) {
			int	len = ::MultiByteToWideChar(CP_UTF8, 0, sql, -1, NULL, 0);
			if (len > 0) {
				tjs_char *str = ret.AllocBuffer(len);
				::MultiByteToWideChar(CP_UTF8, 0, sql, -1, str, len);
			}
		}
		return ret;
	}

	void reset() {
		sqlite3_reset(stmt);
		bindColumnNo = 0;
	}

	static tjs_error bind(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (numparams > 1) {
			self->bindColumnNo = self->getColumn(*params[1]);
		}
		::bind(self->stmt, *params[0], self->bindColumnNo++);
		return TJS_S_OK;
	}
	
	bool step() {
		return sqlite3_step(stmt) == SQLITE_ROW;
	}

	int getDataCount() const {
		return sqlite3_data_count(stmt);
	}

	int getCount() const {
		return sqlite3_column_count(stmt);
	}

	bool isNull(tTJSVariant column) const {
		return sqlite3_column_type(stmt, getColumn(column)) == SQLITE_NULL;
	}

	int getType(tTJSVariant column) const {
		return sqlite3_column_type(stmt, getColumn(column));
	}

	ttstr getName(tTJSVariant column) const {
		return (const tjs_char *)sqlite3_column_name16(stmt, getColumn(column));
	}
	
	static tjs_error get(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		int col = self->getColumn(*params[0]);
		if (sqlite3_column_type(self->stmt, col) == SQLITE_NULL) {
			if (numparams > 1) {
				*result = *params[1];
			} else {
				result->Clear();
			}
		} else {
			::getColumn(self->stmt, *result, col);
		}
		return TJS_S_OK;
	}
	
	/**
	 * �C���X�^���X�����t�@�N�g��
	 */
	static tjs_error factory(SqliteStatement **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (params[0]->AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Sqlite", NULL) != TJS_S_TRUE) {
			TVPThrowExceptionMessage(L"InvalidObject");
		}
		*result = new SqliteStatement(*params[0], numparams > 1 ? params[1]->GetString() : NULL);
		return S_OK;
	}
	
protected:

	/**
	 * �J�����ԍ����擾
	 * @param column �J�����w��
	 * @return �J�����ԍ�
	 */
	int getColumn(const tTJSVariant &column) const {
		if (column.Type() == tvtString) {
			const tjs_char *col = column.GetString();
			if (stmt) {
				int count = sqlite3_column_count(stmt);
				for (int i=0; i<count; i++) {
					if (wcscmp(col, (const tjs_char *)sqlite3_column_name16(stmt, i)) == 0)  {
						return i;
					}
				}
			}
			return 0;
		}
		return column;
	}
	
private:
	tTJSVariant sqlite;
	sqlite3_stmt *stmt;
	int bindColumnNo;
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(SqliteStatement) {
	ENUM(SQLITE_INTEGER);
	ENUM(SQLITE_FLOAT);
	ENUM(SQLITE_TEXT);
	ENUM(SQLITE_BLOB);
	ENUM(SQLITE_NULL);
	Factory(&ClassT::factory);
	NCB_METHOD(open);
	NCB_METHOD(close);
	NCB_PROPERTY_RO(sql, getSql);
	NCB_METHOD(reset);
	RawCallback(TJS_W("bind"), &Class::bind, 0);
	NCB_METHOD(step);
	NCB_PROPERTY_RO(dataCount, getDataCount);
	NCB_PROPERTY_RO(count, getCount);
	NCB_METHOD(isNull);
	NCB_METHOD(getType);
	NCB_METHOD(getName);
	RawCallback(TJS_W("get"), &Class::get, 0);
};


// --------------------------------------------------------------------------

static void
initSqlite()
{
	sqlite3_vfs_register(getXp3Vfs(), 0);
}

static void
doneSqlite()
{
	sqlite3_vfs_unregister(getXp3Vfs());
}

NCB_PRE_REGIST_CALLBACK(initSqlite);
NCB_POST_UNREGIST_CALLBACK(doneSqlite);
