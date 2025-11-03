#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define da_append(xs, x)                                                       \
  do {                                                                         \
    (xs).size += 1;                                                            \
    if ((xs).size > (xs).capacity) {                                           \
      if ((xs).capacity <= 0)                                                  \
        (xs).capacity = (xs).size;                                             \
      (xs).capacity *= 2;                                                      \
      (xs).items = realloc((xs).items, (xs).capacity * sizeof(*(xs).items));   \
    }                                                                          \
    (xs).items[(xs).size - 1] = (x);                                           \
  } while (0)

#define da_reserve_capacity(xs, cap)                                           \
  do {                                                                         \
    if ((xs).size <= 0) {                                                      \
      (xs).capacity = cap;                                                     \
      (xs).size = 0;                                                           \
      (xs).items = realloc((xs).items, (xs).capacity * sizeof(*(xs).items));   \
    }                                                                          \
  } while (0)

ssize_t getdelims(char **restrict linep, size_t *restrict linecapp,
                  char *delims, FILE *restrict stream) {
  ssize_t size = 0;
  char current;
  while ((current = fgetc(stream)) != EOF) {
    bool gotDelim = strchr(delims, current) != NULL;
    if (gotDelim && size > 0) {
      ungetc(current, stream);
      break;
    }
    size += 1;
    if (size > (ssize_t)*linecapp) {
      if (*linecapp <= 0)
        *linecapp = size;
      *linecapp *= 2;
      *linep = realloc(*linep, *linecapp);
    }
    (*linep)[size - 1] = current;
    if (gotDelim) {
      break;
    }
  }
  if (size <= 0) {
    return -1;
  }
  if (size + 1 > (ssize_t)*linecapp) {
    *linecapp *= 2;
    *linep = realloc(*linep, *linecapp);
  }
  (*linep)[size] = 0;
  return size;
}

typedef struct {
  char *delims;
  char *passthroughDelims;
  FILE *file;

  char *token;
  size_t tokenCap;
} tokenizer_t;

int tokenizerInit(tokenizer_t *tp, char *fileName, char *delims,
                  char *passthroughDelims) {
  tp->file = fopen(fileName, "r");
  if (!tp->file) {
    return -1;
  }
  tp->delims = delims;
  tp->passthroughDelims = passthroughDelims;

  tp->token = NULL;
  tp->tokenCap = 0;
  return 1;
}

void tokenizerDeinit(tokenizer_t *tp) { fclose(tp->file); }

ssize_t tokenizerNextToken(tokenizer_t *tp) {
  ssize_t tokenSize = 0;
  while (true) {
    tokenSize = getdelims(&tp->token, &tp->tokenCap, tp->delims, tp->file);
    bool isInDelims = strchr(tp->delims, *tp->token);
    bool isPassthroughDelim =
        isInDelims && strchr(tp->passthroughDelims, *tp->token);
    bool isFullToken = tokenSize > 0 && !isInDelims;
    bool isEnd = tokenSize < 0;
    if (isPassthroughDelim || isFullToken || isEnd) {
      return tokenSize;
    }
  }
}

typedef enum {
  R,
  L,
  P,
  H,
  Tmachine_op_size,
} Tmachine_op_type_t;

typedef struct {
    Tmachine_op_type_t opType;
    char operand;
} Tmachine_op_t;

Tmachine_op_t isTmachineOpT(const char *opLiteral, const size_t opLiteralSize) {
  if (!opLiteral)
    return (Tmachine_op_t){ .opType = -1 };
  if (opLiteralSize == 1) {
    switch (*opLiteral) {
    case 'R': {
      return (Tmachine_op_t){.opType = R};
    }
    case 'L': {
      return (Tmachine_op_t){.opType = L};
    }
    case 'H': {
      return (Tmachine_op_t){.opType = H};
    }
    case 'E': {
      return (Tmachine_op_t){.opType = P, .operand = 0};
    }
    default: {
      return (Tmachine_op_t){.opType = -1};
    }
    }
  } else if (opLiteralSize == 2) {
    switch (*opLiteral) {
    case 'P':
      return (Tmachine_op_t){.opType = P, .operand = opLiteral[1]};
    default:
      return (Tmachine_op_t){.opType = -1};
    }
  } else {
    return (Tmachine_op_t){.opType = -1};
  }
}

typedef struct {
  char *items;
  size_t capacity;
  size_t size;
} dstr_t;

typedef struct {
  Tmachine_op_t *items;
  size_t capacity;
  size_t size;
} Tmachine_ops_t;

typedef struct {
  int state;
  int symbol;
  Tmachine_ops_t ops;
  int endState;
} Tmachine_def_t;

typedef struct {
  Tmachine_def_t *items;
  size_t capacity;
  size_t size;
} Tmachine_defs_t;

typedef struct {
  char **items;
  size_t size;
  size_t capacity;
} states_table_t;

int findStateIndex(const states_table_t *states, char *stateName) {
  for (size_t i = 0; i < states->size; i++) {
    if (strcmp(stateName, states->items[i]) == 0) {
      return i;
    }
  }
  return -1;
}

int getNextOp(tokenizer_t *tp, Tmachine_defs_t *defs, states_table_t *states) {
  Tmachine_def_t def = {0};
  ssize_t tokenSize = tokenizerNextToken(tp);
  if (tokenSize < 0) {
    return 0;
  }

  if (tokenSize <= 0 || !isalpha(*tp->token)) {
    fprintf(stderr, "[ERROR] invalid state name: %s\n", tp->token);
    return -1;
  }
  int stateIndex = findStateIndex(states, tp->token);
  if (stateIndex < 0) {
    char *dupToken = strndup(tp->token, tokenSize);
    da_append(*states, dupToken);
    stateIndex = states->size - 1;
  }
  def.state = stateIndex;

  tokenSize = tokenizerNextToken(tp);
  if (tokenSize != 1) {
    fprintf(stderr, "[ERROR] invalid value for symbol: %s\n", tp->token);
    return -1;
  }
  if (*tp->token == '*') {
    def.symbol = -1;
  } else if (*tp->token == '.') {
    def.symbol = 0;
  } else {
    def.symbol = *tp->token;
  }

  tokenSize = tokenizerNextToken(tp);
  Tmachine_op_t machineOp = {.opType = -1 };
  if (tokenSize == 1 && *tp->token == '[') {
    while (!((tokenSize = tokenizerNextToken(tp)) == 1 && *tp->token == ']')) {
      if ((machineOp = isTmachineOpT(tp->token, tokenSize)).opType >= 0) {
        da_append(def.ops, machineOp);
      } else {
        fprintf(stderr, "[ERROR] invalid value for op: %s\n", tp->token);
        return -1;
      }
    }
  } else if ((machineOp = isTmachineOpT(tp->token, tokenSize)).opType >= 0) {
    da_append(def.ops, machineOp);
  } else {
    fprintf(stderr, "[ERROR] invalid value for op: %s\n", tp->token);
    return -1;
  }

  tokenSize = tokenizerNextToken(tp);
  if (tokenSize <= 0 || !isalpha(*tp->token)) {
    fprintf(stderr, "[ERROR] invalid end state name: %s\n", tp->token);
    return -1;
  }
  int endStateIndex = findStateIndex(states, tp->token);
  if (endStateIndex < 0) {
    char *dupToken = strndup(tp->token, tokenSize);
    da_append(*states, dupToken);
    endStateIndex = states->size - 1;
  }
  def.endState = endStateIndex;

  tokenSize = tokenizerNextToken(tp);
  if (tokenSize >= 0 && *tp->token != '\n') {
    fprintf(stderr, "[ERROR] invalid tokens at the end of a line \"%s\"\n", tp->token);
    return -1;
  }

  da_append(*defs, def);
  return true;
}

typedef struct {
  dstr_t tape;
  Tmachine_defs_t definitions;
  states_table_t* stateTable;

  int currentState;
} Tmachine_t;

int TmachineInit(Tmachine_t *tm, Tmachine_defs_t defs, states_table_t* states) {
  *tm = (Tmachine_t){0};
  da_reserve_capacity(tm->tape, 256);
  for (size_t i = 0; i < tm->tape.capacity; i++) {
    tm->tape.items[i] = 0;
  }
  tm->definitions = defs;
  tm->stateTable = states;
  return 0;
}

void TmachineDumpTape(Tmachine_t* tm, int length) {
    size_t cursorPos = 1 + 4 * tm->tape.size;
    for (int i = 0; i < cursorPos; i++) {
        printf(" ");
    }
    printf("*\n");
  for (size_t i = 0; i < (length >= 0 ? length : tm->tape.capacity); i++) {
    printf(" %c |", tm->tape.items[i]);
  }
}

void TmachineHalt(Tmachine_t *tm) {
  tm->currentState = -1;
  TmachineDumpTape(tm, 256);
}

bool TmachineStep(Tmachine_t *tm) {
  if (tm->tape.size >= tm->tape.capacity) {
    TmachineHalt(tm);
    return false;
  }
  int currentSymbol = tm->tape.items[tm->tape.size];
  int currentState = tm->currentState;
  Tmachine_def_t *item = NULL;
  for (size_t i = 0; i < tm->definitions.size; i++) {
    Tmachine_def_t *iitem = &tm->definitions.items[i];
    if (iitem->state == currentState &&
        (iitem->symbol == currentSymbol || iitem->symbol == -1)) {
      item = iitem;
      break;
    }
  }
  if (!item) {
    fprintf(stderr, "[ERROR] cannot find definition for state = %s and symbol = %c\n", tm->stateTable->items[currentState], currentSymbol);
    return false;
  }

  for (size_t j = 0; j < item->ops.size; j++) {
    Tmachine_op_t op = item->ops.items[j];
    switch (op.opType) {
    case R: {
      tm->tape.size += 1;
      break;
    }
    case L: {
      tm->tape.size -= 1;
      break;
    }
    case P: {
      tm->tape.items[tm->tape.size] = op.operand;
      break;
    }
    case H: {
      TmachineHalt(tm);
      return false;
    }
    case Tmachine_op_size: {
      return false;
    }
    }
  }
  tm->currentState = item->endState;

  printf("[LOG] state = %s | symbol = %c | pos = %zu\n",
         tm->stateTable->items[tm->currentState], tm->tape.items[tm->tape.size],
         tm->tape.size);
  TmachineDumpTape(tm, 16);
  return true;
}

char* TmachineOpDump(Tmachine_op_t op) {
  switch (op.opType) {
  case R: {
    return "R";
  }
  case L: {
    return "L";
  }
  case P: {
    return "PX";
  }
  case H: {
    return "H";
  }
  default: return "N/A";
  }
}

void TmachineDumpDef(Tmachine_t* tm) {
    for (size_t i = 0; i < tm->definitions.size; i++) {
        if (tm->stateTable) {
            printf("def #%zu :: %s + %c => [", i, tm->stateTable->items[tm->definitions.items[i].state], tm->definitions.items[i].symbol);
            for (size_t op_i = 0; op_i < tm->definitions.items[i].ops.size; op_i++) {
                printf("%s, ", TmachineOpDump(tm->definitions.items[i].ops.items[op_i]));
            }
            printf("] => %s\n", tm->stateTable->items[tm->definitions.items[i].endState]);
        } else {
            printf("def #%zu :: %d + %c => [", i, tm->definitions.items[i].state, tm->definitions.items[i].symbol);
            for (size_t op_i = 0; op_i < tm->definitions.items[i].ops.size; op_i++) {
                printf("%s, ", TmachineOpDump(tm->definitions.items[i].ops.items[op_i]));
            }
            printf("] => %d\n", tm->definitions.items[i].endState);
        }
    }
}

int main(int argc, char** argv) {
  tokenizer_t tokenizer;
  if (argc < 2) {
    fprintf(stderr, "[ERROR] provide a file path to trng program\n");
    return -1;
  }
  if (tokenizerInit(&tokenizer, argv[1], " \n[],", "\n[]") < 0) {
    fprintf(stderr, "ERROR: unable to init tokenizer\n");
    return -1;
  }
  Tmachine_defs_t defs = {0};
  states_table_t states = {0};
  int nextOpStatus = 0;
  while ((nextOpStatus = getNextOp(&tokenizer, &defs, &states)) > 0) {
  }
  tokenizerDeinit(&tokenizer);
  Tmachine_t machine = {0};
  TmachineInit(&machine, defs, &states);
  TmachineDumpDef(&machine);
  while (TmachineStep(&machine)) {
    getc(stdin);
  }
  return 0;
}
