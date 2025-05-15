N = 210
P_L = 1/21
P_F = 20/21
P_L_shy = 0.4
P_L_nshy = 1 - 0.4
P_F_shy = 0.1
P_F_nshy = 1 - 0.1
P_shy = (P_L_shy * P_L + P_F_shy * P_F)

print(f"{N * P_shy}")
P_nshy = (P_L_nshy * P_L + P_F_nshy * P_F)
print(f"{N * P_nshy}")

P_that_shy_is_librarian = P_L_shy * P_L / (P_shy)

# 1. pick random guy, he is 5% librarian and 95% farmer
# 2. there are 40% of shy librarians and 10% of shy farmers
# 3. if we pick a librarian (5%) then he is 40% shy
#    if we pick a farmer (95%) then he is 10% shy
P_picking_shy = P_shy

# What is the P of picking shy librarian?
P_shy_librarian = P_L * P_L_shy
P_shy_farmer = P_F * P_F_shy
P_nshy_librarian = P_L * P_L_nshy
P_nshy_farmer = P_F * P_F_nshy

# We know that guy is shy. What's the P that he is librarian?
N_shy = P_shy * N

# What the librarian/farmer distribution among shy guys?
N_shy_librarian = P_shy_librarian * N
N_shy_farmer = P_shy_farmer * N

P_shy_is_librarian = N_shy_librarian / N_shy
P_shy_is_farmer = N_shy_farmer / N_shy

def P_of_LIBRARIAN_if_SHY(P_LIBRARIAN, P_LIBRARIAN_IS_SHY, P_SHY):
    P_SHY_LIBRARIAN = P_LIBRARIAN * P_LIBRARIAN_IS_SHY
    return P_SHY_LIBRARIAN / P_SHY

# P(H | E) = P(H) * P(E | H) / P(E)
def P_of_H_if_E(P_H, P_E_if_H, PE):
    return P_H * P_E_if_H / PE

# P(A | B) means probability of A if B
# probability of guy being librarian if shy = P(Librarian | Shy)
# probability of guy being shy if librarian = P(Shy | Librarian) = P_L_shy
# P(A and B) = P(B) * P(A | B) = P(B) * P(B | A)

# Evidence should NOT DETERMINE beliefs but UPDATE them
# When is Bayes T useful?
# When we want to know something based on facts of this something.
# For example we meet a guy
# He is 5% librarian and 95% farmer
# 
# He told one fact about him: he is shy
# What does it change for us?
# We now update the probability distribution to:
# He is 16% librarian and 83% farmer

P_librarian = P_L

# FACT: he is shy
P_librarian = P_of_H_if_E(P_librarian, P_L_shy, P_shy)
print(P_librarian)

# FACT: he is wearing glasses
P_farmer_with_glasses = 0.25
P_librarian_with_glasses = 0.81
P_with_glasses = (P_farmer_with_glasses * P_L + P_farmer_with_glasses * P_F)

P_librarian = P_of_H_if_E(P_librarian, P_librarian_with_glasses, P_with_glasses)
print(P_librarian)

# FACT: he is wearing pants
P_farmer_with_pants = 1.0
P_librarian_with_pants = 1.0
P_with_pants = (P_farmer_with_pants * P_L + P_farmer_with_pants * P_F)
P_librarian = P_of_H_if_E(P_librarian, P_librarian_with_pants, P_with_pants)
print(P_librarian)
# but that fact is irrelevant and doesn't update P_librarian at all

# FACT: he is driving truc
P_farmer_with_truc = 0.9
P_librarian_with_truc = 0.01
P_with_truc = (P_farmer_with_truc * P_L + P_farmer_with_truc * P_F)
P_librarian = P_of_H_if_E(P_librarian, P_librarian_with_truc, P_with_truc)
print(P_librarian)
# P_librarian is now less then 1% because no librarians are driving truc

















