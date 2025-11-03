dat {
  storage[m];
  registry_stack[n];

  search(k) {
    if (registry[storage[k].registry_idx]) {
      return storage[k];
    } else {
      return nil;
    }
  }

  insert(x) {
    if (registry_stack[storage[x.key].registry_idx].key == x.key) {
      
    }
  }

  delete(x) {

  }
}
